#include "Shooter.h"
#include "PrototypeCharacter.h"
#include "EnemyActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h" // 캐릭터의 GetMesh()가 반환하는 타입


UShooter::UShooter()
{
	PrimaryComponentTick.bCanEverTick = true;

	GrabVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrabVisualMesh"));
	GrabVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrabVisualMesh->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Game/Meshes/SM_GrabCylinder.SM_GrabCylinder"));
	if (MeshAsset.Succeeded())
		GrabVisualMesh->SetStaticMesh(MeshAsset.Object);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	RotationConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RotationConstraint"));
}

void UShooter::BeginPlay()
{
	Super::BeginPlay();

	OwnerChar = Cast<APrototypeCharacter>(GetOwner());
	FollowCamera = OwnerChar ? OwnerChar->FindComponentByClass<UCameraComponent>() : nullptr;

	if (OwnerChar)
	{
		GrabVisualMesh->AttachToComponent(
			OwnerChar->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
		RotationConstraint->AttachToComponent(
			OwnerChar->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);

		GrabVisualMesh->RegisterComponent();
		PhysicsHandle->RegisterComponent();
		RotationConstraint->RegisterComponent();
	}

	/* 컴포넌트 Tick 을 Actor Tick 뒤로 미룸 */
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}


/*-------------------------- Tick ---------------------------*/
void UShooter::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTick)
{
	Super::TickComponent(DeltaTime, TickType, ThisTick);

	if (!OwnerChar) return;

	UpdateLineTrace();
	UpdateGrabbedPhysics(DeltaTime);
	UpdateGrabbedNonPhysics(DeltaTime);
	UpdateMissedGrabVisual(DeltaTime);
}

FVector UShooter::GetActualLineTraceStartLocation() const
{
	// OwnerChar 자체는 UpdateLineTrace와 같은 호출부에서 유효성 검사를 하고 넘어온다고 가정합니다.
	// 만약 여기서도 OwnerChar가 nullptr일 수 있다면, 가장 먼저 체크해야 합니다.
	if (!OwnerChar)
	{
		// UE_LOG(LogTemp, Error, TEXT("GetActualLineTraceStartLocation called with null OwnerChar!"));
		if (GetOwner()) // 컴포넌트의 직접적인 소유주 액터가 있다면 그 위치를 반환
		{
			return GetOwner()->GetActorLocation();
		}
		return FVector::ZeroVector; // 최후의 폴백
	}

	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);

	if (ProtoChar) // OwnerChar가 APrototypeCharacter로 성공적으로 캐스팅되었는지 확인
	{
		// CharacterMuzzleSocketName이 설정되어 있고, GetMesh()가 유효한지 확인
		if (ProtoChar->GetMesh() && ProtoChar->CharacterMuzzleSocketName != NAME_None)
		{
			if (ProtoChar->GetMesh()->DoesSocketExist(ProtoChar->CharacterMuzzleSocketName))
			{
				return ProtoChar->GetMesh()->GetSocketLocation(ProtoChar->CharacterMuzzleSocketName); // 경로 1: 소켓 위치 반환
			}
			else
			{
				// 소켓 이름은 있지만 실제 소켓이 없는 경우
				// UE_LOG(LogTemp, Warning, TEXT("UShooter: Socket '%s' not found on OwnerChar's mesh. Using character location + offset."), *ProtoChar->CharacterMuzzleSocketName.ToString());
				return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f); // 경로 2: 폴백 1 (캐릭터 위치 + 오프셋)
			}
		}
		else
		{
			// MuzzleSocketName이 설정되지 않았거나 GetMesh()가 유효하지 않은 경우
			// UE_LOG(LogTemp, Warning, TEXT("UShooter: OwnerChar's mesh is null or CharacterMuzzleSocketName is None. Using character location + offset."));
			return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f); // 경로 3: 폴백 2 (캐릭터 위치 + 오프셋)
		}
	}

	// OwnerChar가 APrototypeCharacter 타입이 아닌 경우 (캐스팅 실패)
	// UE_LOG(LogTemp, Warning, TEXT("UShooter: OwnerChar is not of type APrototypeCharacter. Using character location + offset."));
	return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f); // 경로 4: 최종 폴백 (캐릭터 위치 + 오프셋)
}

/*--------------------- 입력 래퍼 (본문 이동) ---------------------*/
void UShooter::Grab() {
	if (bIsLineTraceHit)
	{
		UPrimitiveComponent* HitComponent = CachedHitResult.GetComponent();
		if (!HitComponent)  return;

		if (HitComponent->IsSimulatingPhysics())
		{
			float CurrentDistance = FVector::Dist(OwnerChar->GetActorLocation(), HitComponent->GetComponentLocation());
			if (CurrentDistance < GrabMinDistance || CurrentDistance > GrabMaxDistance) return;

			HitComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			HitComponent->SetEnableGravity(false);

			GrabbedObjectDistance = CurrentDistance;
			GrabbedComponent = HitComponent;

			PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetComponentLocation());
			PhysicsHandle->InterpolationSpeed = 100.0f;
			PhysicsHandle->SetTargetLocation(OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance);

			RotationConstraint->SetWorldLocation(HitComponent->GetComponentLocation());
			RotationConstraint->SetConstrainedComponents(nullptr, NAME_None, HitComponent, NAME_None);
			RotationConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);

			DrawDebugSphere(GetWorld(), HitComponent->GetComponentLocation(), 20.f, 12, FColor::Yellow, false, 1.0f);
			OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{

			FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
			CurrentVelocity.X = 0.0f;  // Z축 힘만 초기화
			CurrentVelocity.Y = 0.0f;  // Z축 힘만 초기화
			OwnerChar->GetCharacterMovement()->Velocity = CurrentVelocity;
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor(128, 0, 128), false, 1.0f, 0, 2.0f);
			NonPhysicsHitLocation = CachedHitResult.ImpactPoint;
			NonPhysicsGrabDistance = FVector::Dist(OwnerChar->GetActorLocation(), NonPhysicsHitLocation);
			bIsGrabbingNonPhysics = true;
		}
	}
	if (!bIsLineTraceHit || !CachedHitResult.GetComponent())
	{
		MissedGrabTarget = LineEnd;
		MissedGrabTimer = MissedGrabDuration;
		bShowMissedGrabVisual = true;

		if (GrabVisualMesh)
		{
			GrabVisualMesh->SetVisibility(true);
		}
		return;
	}

}
void UShooter::Release() {
	if (GrabbedComponent)
	{
		GrabbedComponent->SetEnableGravity(true);
		GrabbedComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		GrabbedComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		RotationConstraint->BreakConstraint();
		PhysicsHandle->ReleaseComponent();
		GrabbedComponent = nullptr;

	}
	if (bIsGrabbingNonPhysics) {
		FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = 0.0f;  // Z축 힘만 초기화

	}
	bIsGrabbingNonPhysics = false;
	OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
	if (GrabVisualMesh)
	{
		GrabVisualMesh->SetVisibility(false);
	}

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bNonPhysicsCollisionOccurred = false; // 충돌 리셋

}

void UShooter::ScrollUp() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance + ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// 실린더 업데이트
		UpdateGrabVisualMesh();
	}
}
void UShooter::ScrollDown() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance - ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// 실린더 업데이트
		UpdateGrabVisualMesh();
	}
}
void UShooter::RightMouseDown() { bIsRightMouseButtonDown = true; }
void UShooter::RightMouseUp() { bIsRightMouseButtonDown = false; }

/*------------------- 내부 헬퍼 (본문 동일) --------------------*/
void UShooter::UpdateLineTrace()
{
	if (!OwnerChar) return;

	LineStart = GetActualLineTraceStartLocation(); // 수정된 헬퍼 함수가 정확한 소켓 위치를 반환
	FVector CharacterForwardDirection = OwnerChar->GetActorForwardVector(); // 캐릭터 정면 방향
	LineEnd = LineStart + CharacterForwardDirection * GrabMaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerChar);
	if (CachedHitResult.GetActor() && CachedHitResult.GetActor()->ActorHasTag("Enemy"))
	{
		Params.AddIgnoredActor(CachedHitResult.GetActor());
	}

	bIsLineTraceHit = GetWorld()->LineTraceSingleByChannel(
		CachedHitResult, LineStart, LineEnd, ECC_PhysicsBody, Params);

	FColor LineColor = bIsLineTraceHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, false, 0.0f, 0, 2.0f);
}

void UShooter::UpdateGrabbedPhysics(float DeltaTime)
{
	if (!GrabbedComponent) return;

	FVector TargetLocation = OwnerChar->GetActorLocation() +
		OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;

	PhysicsHandle->InterpolationSpeed = 100.0f;
	PhysicsHandle->SetTargetLocation(TargetLocation);

	if (GrabVisualMesh)
	{
		FVector Start = OwnerChar->GetActorLocation();
		FVector End = TargetLocation;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		FVector Mid = (Start + End) * 0.5f;

		FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(
			CameraDir, RopeDir.GetSafeNormal())));
		float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;
		float ScaleZ = Length / 100.0f;

		GrabVisualMesh->SetVisibility(true);
		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
	}
}
void UShooter::UpdateGrabbedNonPhysics(float DeltaTime)
{
	if (!bIsGrabbingNonPhysics || !OwnerChar) return;

	FVector Delta = OwnerChar->GetActorLocation() - NonPhysicsHitLocation;
	OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;

	FVector RopeVector = Delta;
	float CurrentDistance = RopeVector.Size();
	FVector Direction = RopeVector.GetSafeNormal();
	FVector TargetLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;

	FVector IdealLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
	FVector CorrectionVector = IdealLocation - OwnerChar->GetActorLocation();
	FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
	CurrentVelocity += CorrectionVector * CorrectionStrength * DeltaTime;

	if (OwnerChar->Controller)
	{
		FVector Forward = OwnerChar->Controller->GetControlRotation().Vector();
		FVector Right = FRotationMatrix(OwnerChar->Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);

		FVector InputVector =
			Forward * OwnerChar->GetInputAxisValue("MoveForward") +
			Right * OwnerChar->GetInputAxisValue("MoveRight");

		InputVector.Z = 0.0f;
		CurrentVelocity += InputVector * SwingForce * DeltaTime;
	}

	OwnerChar->GetCharacterMovement()->Velocity = CurrentVelocity;

	if (GrabVisualMesh)
	{
		FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
			FVector::DotProduct(CameraDir, RopeVector.GetSafeNormal())));
		float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;
		float ScaleZ = RopeVector.Size() / 100.0f;
		FVector Mid = (OwnerChar->GetActorLocation() + NonPhysicsHitLocation) * 0.5f;

		float ConstantThickness = 0.5f;
		GrabVisualMesh->SetVisibility(true);
		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeVector).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
	}

	if (bIsRightMouseButtonDown)
	{
		NonPhysicsGrabDistance -= NonPhysicsDistanceReduceSpeed * DeltaTime;
		NonPhysicsGrabDistance = FMath::Max(NonPhysicsGrabDistance, 100.0f);

		FVector PullTarget = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
		FVector NewLocation = FMath::VInterpTo(OwnerChar->GetActorLocation(), PullTarget, DeltaTime, 800.0f);
		OwnerChar->SetActorLocation(NewLocation, true);
	}
	else
	{
		FVector NewLocation = FMath::VInterpTo(OwnerChar->GetActorLocation(), TargetLocation, DeltaTime, 100.0f);
		OwnerChar->SetActorLocation(NewLocation, true);
	}

	float MaxAllowedDistance = NonPhysicsGrabDistance;
	FVector DesiredLocation = OwnerChar->GetActorLocation();
	FVector OffsetFromCenter = DesiredLocation - NonPhysicsHitLocation;
	float OffsetLength = OffsetFromCenter.Size();

	if (OffsetLength > MaxAllowedDistance)
	{
		OffsetFromCenter = OffsetFromCenter.GetSafeNormal() * MaxAllowedDistance;
		FVector CorrectedLocation = NonPhysicsHitLocation + OffsetFromCenter;
		OwnerChar->SetActorLocation(CorrectedLocation, false);
	}

	DrawDebugLine(GetWorld(), OwnerChar->GetActorLocation(), NonPhysicsHitLocation, FColor::Purple, false, 0.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, 15.0f, 12, FColor::Yellow, false, 0.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, MaxAllowedDistance, 32, FColor::Green, false, 0.0f, 0, 0.5f);
}
void UShooter::UpdateMissedGrabVisual(float DeltaTime)
{
	if (!bShowMissedGrabVisual || !GrabVisualMesh || !OwnerChar) return;

	MissedGrabTimer -= DeltaTime;

	if (MissedGrabTimer <= 0.0f)
	{
		bShowMissedGrabVisual = false;
		GrabVisualMesh->SetVisibility(false);
	}
	else
	{
		FVector Start = OwnerChar->GetActorLocation();
		FVector End = MissedGrabTarget;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		FVector Mid = (Start + End) * 0.5f;

		FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
			FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
		float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

		float ScaleZ = Length / 100.0f;

		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
		GrabVisualMesh->SetVisibility(true);
	}
}
void UShooter::UpdateGrabVisualMesh()
{
	if (!GrabVisualMesh || !GrabbedComponent || !OwnerChar) return;

	FVector Start = OwnerChar->GetActorLocation();
	FVector End = Start + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
	FVector RopeDir = End - Start;
	float Length = RopeDir.Size();
	FVector Mid = (Start + End) * 0.5f;

	float ScaleZ = Length / 100.0f;
	FRotator Rotation = FRotationMatrix::MakeFromZ(RopeDir).Rotator();

	FVector CameraDir = FollowCamera->GetForwardVector();
	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
		FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
	float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

	GrabVisualMesh->SetWorldLocation(Mid);
	GrabVisualMesh->SetWorldRotation(Rotation);
	GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
	GrabVisualMesh->SetVisibility(true);
}
