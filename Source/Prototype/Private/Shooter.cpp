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
	NonPhysicsGrabRotationInterpSpeed = 360.0f; // 초당 360도 (값 조절 가능)
	MaxGrabStretchTolerance = 50.0f; // 예시: 유지 거리에서 50유닛까지는 더 늘어나도 괜찮음 (값 조절 가능)
	GrabDistanceAdaptThreshold = 15.0f; // 예시: 5 유닛. 이 값보다 더 가까워져야 거리 업데이트

	GrabVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrabVisualMesh"));
	GrabVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrabVisualMesh->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Game/Meshes/Cube.Cube"));
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

// Shooter.cpp
FVector UShooter::GetActualLineTraceStartLocation() const
{
	if (!OwnerChar) { /*...*/ return FVector::ZeroVector; }
	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);
	if (ProtoChar)
	{
		if (ProtoChar->GetMesh() && ProtoChar->CharacterMuzzleSocketName != NAME_None) // CharacterMainSocketName 사용
		{
			if (ProtoChar->GetMesh()->DoesSocketExist(ProtoChar->CharacterMuzzleSocketName))
			{
				return ProtoChar->GetMesh()->GetSocketLocation(ProtoChar->CharacterMuzzleSocketName); // 캐릭터 주 메쉬의 "Buster" 소켓
			}
			// UE_LOG(LogTemp, Warning, TEXT("Socket '%s' not found on OwnerChar's mesh."), *ProtoChar->CharacterMainSocketName.ToString());
			return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f);
		}
		// UE_LOG(LogTemp, Warning, TEXT("OwnerChar's mesh or CharacterMainSocketName is invalid."));
		return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f);
	}
	return OwnerChar ? OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f) : FVector::ZeroVector;
}
// Shooter.cpp
FVector UShooter::GetVisualCylinderStartLocation() const
{
	if (!OwnerChar)
	{
		// UE_LOG(LogTemp, Error, TEXT("GetVisualCylinderStartLocation: OwnerChar is NULL!"));
		if (GetOwner()) return GetOwner()->GetActorLocation(); // 안전장치: 컴포넌트 소유주 액터 위치
		return FVector::ZeroVector; // 최후의 폴백
	}

	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);

	// AttachedBusterMeshComponent (예: SM_Rachel_Buster)와 NozzleSocketNameOnBusterMesh 사용
	if (ProtoChar && ProtoChar->AttachedBusterMeshComponent)
	{
		UMeshComponent* ActualBusterMesh = Cast<UMeshComponent>(ProtoChar->AttachedBusterMeshComponent);
		if (ActualBusterMesh && ProtoChar->NozzleSocketNameOnBusterMesh != NAME_None && ActualBusterMesh->DoesSocketExist(ProtoChar->NozzleSocketNameOnBusterMesh))
		{
			// 로그 추가: 실제 Nozzle 소켓 위치 반환 확인
			FVector NozzleLocation = ActualBusterMesh->GetSocketLocation(ProtoChar->NozzleSocketNameOnBusterMesh);
			// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: Returning Nozzle Socket Location: %s"), *NozzleLocation.ToString());
			return NozzleLocation; // "Buster 메시"의 "Nozzle" 소켓
		}

		// Nozzle 소켓을 못 찾았거나 이름이 설정 안된 경우, Buster 메시의 원점이라도 반환
		if (ActualBusterMesh)
		{
			// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: Nozzle socket '%s' not found on '%s' or name is None. Returning component origin."), *ProtoChar->NozzleSocketNameOnBusterMesh.ToString(), *ActualBusterMesh->GetName());
			return ActualBusterMesh->GetComponentLocation();
		}
	}

	// AttachedBusterMeshComponent가 설정되지 않은 경우, 최종 폴백으로 물리 트레이스 시작점 사용
	// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: AttachedBusterMeshComponent not set. Visual cylinder falling back to physics LineTraceStart."));
	return GetActualLineTraceStartLocation(); // GetActualLineTraceStartLocation은 "Buster" 소켓 위치를 반환
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
			if (OwnerChar && OwnerChar->GetCharacterMovement())
			{
				OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false; // <--- 추가: 이동 방향으로 자동 회전 끄기
			}
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
	bool bWasGrabbing = GrabbedComponent || bIsGrabbingNonPhysics; // 잡고 있었는지 여부

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bShowMissedGrabVisual = false;
	bNonPhysicsCollisionOccurred = false;

	if (OwnerChar && OwnerChar->GetCharacterMovement())
	{
		OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
		if (bWasGrabbing) // 잡고 있었던 상태를 해제할 때만 회전 복원
		{
			OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = true; // <--- 추가: 이동 방향으로 자동 회전 다시 켜기
		}
	}

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
	// --- 시작: 새로운 로직 - 캐릭터/물체가 외부 요인으로 가까워지면 GrabbedObjectDistance 업데이트 ---
	// 현재 Grab() 및 Scroll() 함수에서 GrabbedObjectDistance와 PhysicsHandle의 TargetLocation이
	// OwnerChar->GetActorLocation()을 기준으로 계산되므로, 여기서도 동일한 기준을 사용합니다.
	 // --- 시작: 수정된 로직 - 캐릭터/물체가 외부 요인으로 '확실히' 가까워지면 GrabbedObjectDistance 업데이트 ---
	float CurrentActualDistanceToObject = FVector::Dist(OwnerChar->GetActorLocation(), GrabbedComponent->GetComponentLocation());

	// 실제 거리가 (현재 설정된 유지 거리 - 임계값)보다 짧아졌을 때만 GrabbedObjectDistance 업데이트
	if (CurrentActualDistanceToObject < (GrabbedObjectDistance - GrabDistanceAdaptThreshold))
	{
		GrabbedObjectDistance = CurrentActualDistanceToObject;
		GrabbedObjectDistance = FMath::Max(GrabbedObjectDistance, GrabMinDistance);
		UE_LOG(LogTemp, Log, TEXT("GrabbedObjectDistance adapted due to significant proximity: %f"), GrabbedObjectDistance);
	}
	// --- 끝: 새로운 로직 ---
	// --- 조준 기반 물체 놓기 로직 ---
	FVector AimCheckStartLocation = GetActualLineTraceStartLocation(); // 라인 트레이스 시작점 (예: Nozzle 소켓)
	FVector AimDirection = OwnerChar->GetActorForwardVector();         // 캐릭터가 바라보는 방향

	// 조준 확인용 라인 트레이스 길이 설정:
	// 현재 물체를 유지하려는 거리(GrabbedObjectDistance)에 약간의 허용치(MaxGrabStretchTolerance)를 더한 값 사용
	float AimCheckLength = GrabbedObjectDistance + MaxGrabStretchTolerance;
	// 최소한 GrabMinDistance보다는 길게 설정 (물체가 매우 가까이 있을 경우 대비)
	if (AimCheckLength < GrabMinDistance) AimCheckLength = GrabMinDistance;

	FVector AimCheckEndLocation = AimCheckStartLocation + AimDirection * AimCheckLength;

	FHitResult AimHitResult;
	FCollisionQueryParams AimParams;
	AimParams.AddIgnoredActor(OwnerChar); // 캐릭터는 무시

	bool bAimHitSomething = GetWorld()->LineTraceSingleByChannel(
		AimHitResult,
		AimCheckStartLocation,
		AimCheckEndLocation,
		ECC_PhysicsBody, // 잡고 있는 물체와 동일한 콜리전 채널 사용
		AimParams);

	bool bStillAimingAtGrabbedObject = false;
	if (bAimHitSomething)
	{
		// 라인 트레이스에 맞은 컴포넌트가 현재 잡고 있는 GrabbedComponent와 동일한지 확인
		if (AimHitResult.GetComponent() == GrabbedComponent)
		{
			bStillAimingAtGrabbedObject = true;
		}
	}

	// 디버그를 위해 조준 확인용 라인 트레이스를 그려볼 수 있습니다.
	 DrawDebugLine(GetWorld(), AimCheckStartLocation, AimCheckEndLocation, bStillAimingAtGrabbedObject ? FColor::Cyan : FColor::Orange, false, 0.0f, 0, 1.0f);

	if (!bStillAimingAtGrabbedObject) // 만약 조준이 잡고 있는 물체에서 벗어났다면
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooter: Aim moved off grabbed object. Releasing."));
		Release(); // 물체를 놓습니다.
		return;    // 물체를 놓았으므로 이 프레임에서는 더 이상 처리할 필요가 없습니다.
	}
	// --- 여기까지 조준 기반 물체 놓기 로직 ---

	FVector TargetLocation = OwnerChar->GetActorLocation() +
		OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;

	PhysicsHandle->InterpolationSpeed = 100.0f;
	PhysicsHandle->SetTargetLocation(TargetLocation);
	
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation(); // <--- 여기를 수정! (Nozzle 소켓 위치)
		FVector End = TargetLocation;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
			return;
		}
		FVector Mid = (Start + End) * 0.5f;

		/*FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(
			CameraDir, RopeDir.GetSafeNormal())));
		float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;*/
		float ConstantMyThickness = 0.1f; // <--- 원하는 고정 두께 값으로 설정

		float ScaleZ = Length / 100.0f;

		GrabVisualMesh->SetVisibility(true);
		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(ConstantMyThickness, ConstantMyThickness, ScaleZ));
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
		FVector Start = GetVisualCylinderStartLocation(); // <--- 여기를 수정! (Nozzle 소켓 위치)
		FVector End = NonPhysicsHitLocation;                  // 실린더 끝점은 고정된 히트 지점
		// (또는 캐릭터 위치로 하려면: OwnerChar->GetActorLocation())

		FVector VisualRopeDir = End - Start; // 비주얼을 위한 방향 벡터
		float Length = VisualRopeDir.Size();
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
		}
		else
		{
			FVector Mid = (Start + End) * 0.5f;
			FVector CameraDir = FollowCamera ? FollowCamera->GetForwardVector() : OwnerChar->GetActorForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
				FVector::DotProduct(CameraDir, VisualRopeDir.GetSafeNormal())));
			// float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f; // 기존 두께 로직
			float ConstantThickness = 0.1f; // 코드에 있던 고정 두께 사용
			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(VisualRopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}
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
	// 캐릭터가 NonPhysicsHitLocation을 바라보도록 회전시키는 로직
	if (OwnerChar->GetCharacterMovement() && !OwnerChar->GetCharacterMovement()->bOrientRotationToMovement) // 자동 회전이 꺼져 있을 때만
	{
		FVector TargetLookAtLocation = NonPhysicsHitLocation;
		FVector CharacterLocation = OwnerChar->GetActorLocation();

		FVector DirectionToTarget = TargetLookAtLocation - CharacterLocation;
		DirectionToTarget.Z = 0.0f; // 수평 회전만 고려 (캐릭터가 위아래로 기울어지지 않도록)

		if (!DirectionToTarget.IsNearlyZero(0.01f)) // 매우 가까우면 회전하지 않음
		{
			FRotator CurrentRotation = OwnerChar->GetActorRotation();
			FRotator TargetRotation = DirectionToTarget.Rotation();

			// 부드러운 회전을 위해 RInterpTo 사용
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, NonPhysicsGrabRotationInterpSpeed);
			OwnerChar->SetActorRotation(NewRotation);
		}
	}

	// GrabVisualMesh 업데이트 로직 (이전과 동일하게 시작점만 GetVisualCylinderStartLocation() 사용)
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = NonPhysicsHitLocation;

		FVector VisualRopeDir = End - Start;
		float Length = VisualRopeDir.Size();
		// ... (이하 GrabVisualMesh 설정 로직은 이전 답변과 동일하게 유지) ...
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
		}
		else
		{
			FVector Mid = (Start + End) * 0.5f;
			FVector CameraDir = FollowCamera ? FollowCamera->GetForwardVector() : OwnerChar->GetActorForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
				FVector::DotProduct(CameraDir, VisualRopeDir.GetSafeNormal())));
			float ConstantThickness = 0.1f; // 두께 값 (이전 코드에서 0.1f 사용)
			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(VisualRopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}
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
		FVector Start = GetVisualCylinderStartLocation(); // <--- 여기를 수정! (Nozzle 소켓 위치)
		FVector End = MissedGrabTarget;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		FVector Mid = (Start + End) * 0.5f;

		FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
			FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
		float Thickness = 0.1f;

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

	FVector Start = GetVisualCylinderStartLocation(); // <--- 여기를 수정! (Nozzle 소켓 위치)
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
