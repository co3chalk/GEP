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
#include "Components/SkeletalMeshComponent.h" // ĳ������ GetMesh()�� ��ȯ�ϴ� Ÿ��


UShooter::UShooter()
{
	PrimaryComponentTick.bCanEverTick = true;
	NonPhysicsGrabRotationInterpSpeed = 360.0f; // �ʴ� 360�� (�� ���� ����)
	MaxGrabStretchTolerance = 50.0f; // ����: ���� �Ÿ����� 50���ֱ����� �� �þ�� ������ (�� ���� ����)
	GrabDistanceAdaptThreshold = 15.0f; // ����: 5 ����. �� ������ �� ��������� �Ÿ� ������Ʈ

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

	/* ������Ʈ Tick �� Actor Tick �ڷ� �̷� */
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
		if (ProtoChar->GetMesh() && ProtoChar->CharacterMuzzleSocketName != NAME_None) // CharacterMainSocketName ���
		{
			if (ProtoChar->GetMesh()->DoesSocketExist(ProtoChar->CharacterMuzzleSocketName))
			{
				return ProtoChar->GetMesh()->GetSocketLocation(ProtoChar->CharacterMuzzleSocketName); // ĳ���� �� �޽��� "Buster" ����
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
		if (GetOwner()) return GetOwner()->GetActorLocation(); // ������ġ: ������Ʈ ������ ���� ��ġ
		return FVector::ZeroVector; // ������ ����
	}

	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);

	// AttachedBusterMeshComponent (��: SM_Rachel_Buster)�� NozzleSocketNameOnBusterMesh ���
	if (ProtoChar && ProtoChar->AttachedBusterMeshComponent)
	{
		UMeshComponent* ActualBusterMesh = Cast<UMeshComponent>(ProtoChar->AttachedBusterMeshComponent);
		if (ActualBusterMesh && ProtoChar->NozzleSocketNameOnBusterMesh != NAME_None && ActualBusterMesh->DoesSocketExist(ProtoChar->NozzleSocketNameOnBusterMesh))
		{
			// �α� �߰�: ���� Nozzle ���� ��ġ ��ȯ Ȯ��
			FVector NozzleLocation = ActualBusterMesh->GetSocketLocation(ProtoChar->NozzleSocketNameOnBusterMesh);
			// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: Returning Nozzle Socket Location: %s"), *NozzleLocation.ToString());
			return NozzleLocation; // "Buster �޽�"�� "Nozzle" ����
		}

		// Nozzle ������ �� ã�Ұų� �̸��� ���� �ȵ� ���, Buster �޽��� �����̶� ��ȯ
		if (ActualBusterMesh)
		{
			// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: Nozzle socket '%s' not found on '%s' or name is None. Returning component origin."), *ProtoChar->NozzleSocketNameOnBusterMesh.ToString(), *ActualBusterMesh->GetName());
			return ActualBusterMesh->GetComponentLocation();
		}
	}

	// AttachedBusterMeshComponent�� �������� ���� ���, ���� �������� ���� Ʈ���̽� ������ ���
	// UE_LOG(LogTemp, Warning, TEXT("GetVisualCylinderStartLocation: AttachedBusterMeshComponent not set. Visual cylinder falling back to physics LineTraceStart."));
	return GetActualLineTraceStartLocation(); // GetActualLineTraceStartLocation�� "Buster" ���� ��ġ�� ��ȯ
}
/*--------------------- �Է� ���� (���� �̵�) ---------------------*/
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
			CurrentVelocity.X = 0.0f;  // Z�� ���� �ʱ�ȭ
			CurrentVelocity.Y = 0.0f;  // Z�� ���� �ʱ�ȭ
			OwnerChar->GetCharacterMovement()->Velocity = CurrentVelocity;
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor(128, 0, 128), false, 1.0f, 0, 2.0f);
			NonPhysicsHitLocation = CachedHitResult.ImpactPoint;
			NonPhysicsGrabDistance = FVector::Dist(OwnerChar->GetActorLocation(), NonPhysicsHitLocation);
			bIsGrabbingNonPhysics = true;
			if (OwnerChar && OwnerChar->GetCharacterMovement())
			{
				OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false; // <--- �߰�: �̵� �������� �ڵ� ȸ�� ����
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
		CurrentVelocity.Z = 0.0f;  // Z�� ���� �ʱ�ȭ

	}
	bIsGrabbingNonPhysics = false;
	OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
	if (GrabVisualMesh)
	{
		GrabVisualMesh->SetVisibility(false);
	}
	bool bWasGrabbing = GrabbedComponent || bIsGrabbingNonPhysics; // ��� �־����� ����

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bShowMissedGrabVisual = false;
	bNonPhysicsCollisionOccurred = false;

	if (OwnerChar && OwnerChar->GetCharacterMovement())
	{
		OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
		if (bWasGrabbing) // ��� �־��� ���¸� ������ ���� ȸ�� ����
		{
			OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = true; // <--- �߰�: �̵� �������� �ڵ� ȸ�� �ٽ� �ѱ�
		}
	}

}

void UShooter::ScrollUp() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance + ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// �Ǹ��� ������Ʈ
		UpdateGrabVisualMesh();
	}
}
void UShooter::ScrollDown() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance - ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// �Ǹ��� ������Ʈ
		UpdateGrabVisualMesh();
	}
}
void UShooter::RightMouseDown() { bIsRightMouseButtonDown = true; }
void UShooter::RightMouseUp() { bIsRightMouseButtonDown = false; }

/*------------------- ���� ���� (���� ����) --------------------*/
void UShooter::UpdateLineTrace()
{
	if (!OwnerChar) return;

	LineStart = GetActualLineTraceStartLocation(); // ������ ���� �Լ��� ��Ȯ�� ���� ��ġ�� ��ȯ
	FVector CharacterForwardDirection = OwnerChar->GetActorForwardVector(); // ĳ���� ���� ����
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
	// --- ����: ���ο� ���� - ĳ����/��ü�� �ܺ� �������� ��������� GrabbedObjectDistance ������Ʈ ---
	// ���� Grab() �� Scroll() �Լ����� GrabbedObjectDistance�� PhysicsHandle�� TargetLocation��
	// OwnerChar->GetActorLocation()�� �������� ���ǹǷ�, ���⼭�� ������ ������ ����մϴ�.
	 // --- ����: ������ ���� - ĳ����/��ü�� �ܺ� �������� 'Ȯ����' ��������� GrabbedObjectDistance ������Ʈ ---
	float CurrentActualDistanceToObject = FVector::Dist(OwnerChar->GetActorLocation(), GrabbedComponent->GetComponentLocation());

	// ���� �Ÿ��� (���� ������ ���� �Ÿ� - �Ӱ谪)���� ª������ ���� GrabbedObjectDistance ������Ʈ
	if (CurrentActualDistanceToObject < (GrabbedObjectDistance - GrabDistanceAdaptThreshold))
	{
		GrabbedObjectDistance = CurrentActualDistanceToObject;
		GrabbedObjectDistance = FMath::Max(GrabbedObjectDistance, GrabMinDistance);
		UE_LOG(LogTemp, Log, TEXT("GrabbedObjectDistance adapted due to significant proximity: %f"), GrabbedObjectDistance);
	}
	// --- ��: ���ο� ���� ---
	// --- ���� ��� ��ü ���� ���� ---
	FVector AimCheckStartLocation = GetActualLineTraceStartLocation(); // ���� Ʈ���̽� ������ (��: Nozzle ����)
	FVector AimDirection = OwnerChar->GetActorForwardVector();         // ĳ���Ͱ� �ٶ󺸴� ����

	// ���� Ȯ�ο� ���� Ʈ���̽� ���� ����:
	// ���� ��ü�� �����Ϸ��� �Ÿ�(GrabbedObjectDistance)�� �ణ�� ���ġ(MaxGrabStretchTolerance)�� ���� �� ���
	float AimCheckLength = GrabbedObjectDistance + MaxGrabStretchTolerance;
	// �ּ��� GrabMinDistance���ٴ� ��� ���� (��ü�� �ſ� ������ ���� ��� ���)
	if (AimCheckLength < GrabMinDistance) AimCheckLength = GrabMinDistance;

	FVector AimCheckEndLocation = AimCheckStartLocation + AimDirection * AimCheckLength;

	FHitResult AimHitResult;
	FCollisionQueryParams AimParams;
	AimParams.AddIgnoredActor(OwnerChar); // ĳ���ʹ� ����

	bool bAimHitSomething = GetWorld()->LineTraceSingleByChannel(
		AimHitResult,
		AimCheckStartLocation,
		AimCheckEndLocation,
		ECC_PhysicsBody, // ��� �ִ� ��ü�� ������ �ݸ��� ä�� ���
		AimParams);

	bool bStillAimingAtGrabbedObject = false;
	if (bAimHitSomething)
	{
		// ���� Ʈ���̽��� ���� ������Ʈ�� ���� ��� �ִ� GrabbedComponent�� �������� Ȯ��
		if (AimHitResult.GetComponent() == GrabbedComponent)
		{
			bStillAimingAtGrabbedObject = true;
		}
	}

	// ����׸� ���� ���� Ȯ�ο� ���� Ʈ���̽��� �׷��� �� �ֽ��ϴ�.
	 DrawDebugLine(GetWorld(), AimCheckStartLocation, AimCheckEndLocation, bStillAimingAtGrabbedObject ? FColor::Cyan : FColor::Orange, false, 0.0f, 0, 1.0f);

	if (!bStillAimingAtGrabbedObject) // ���� ������ ��� �ִ� ��ü���� ����ٸ�
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooter: Aim moved off grabbed object. Releasing."));
		Release(); // ��ü�� �����ϴ�.
		return;    // ��ü�� �������Ƿ� �� �����ӿ����� �� �̻� ó���� �ʿ䰡 �����ϴ�.
	}
	// --- ������� ���� ��� ��ü ���� ���� ---

	FVector TargetLocation = OwnerChar->GetActorLocation() +
		OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;

	PhysicsHandle->InterpolationSpeed = 100.0f;
	PhysicsHandle->SetTargetLocation(TargetLocation);
	
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation(); // <--- ���⸦ ����! (Nozzle ���� ��ġ)
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
		float ConstantMyThickness = 0.1f; // <--- ���ϴ� ���� �β� ������ ����

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
		FVector Start = GetVisualCylinderStartLocation(); // <--- ���⸦ ����! (Nozzle ���� ��ġ)
		FVector End = NonPhysicsHitLocation;                  // �Ǹ��� ������ ������ ��Ʈ ����
		// (�Ǵ� ĳ���� ��ġ�� �Ϸ���: OwnerChar->GetActorLocation())

		FVector VisualRopeDir = End - Start; // ���־��� ���� ���� ����
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
			// float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f; // ���� �β� ����
			float ConstantThickness = 0.1f; // �ڵ忡 �ִ� ���� �β� ���
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
	// ĳ���Ͱ� NonPhysicsHitLocation�� �ٶ󺸵��� ȸ����Ű�� ����
	if (OwnerChar->GetCharacterMovement() && !OwnerChar->GetCharacterMovement()->bOrientRotationToMovement) // �ڵ� ȸ���� ���� ���� ����
	{
		FVector TargetLookAtLocation = NonPhysicsHitLocation;
		FVector CharacterLocation = OwnerChar->GetActorLocation();

		FVector DirectionToTarget = TargetLookAtLocation - CharacterLocation;
		DirectionToTarget.Z = 0.0f; // ���� ȸ���� ��� (ĳ���Ͱ� ���Ʒ��� �������� �ʵ���)

		if (!DirectionToTarget.IsNearlyZero(0.01f)) // �ſ� ������ ȸ������ ����
		{
			FRotator CurrentRotation = OwnerChar->GetActorRotation();
			FRotator TargetRotation = DirectionToTarget.Rotation();

			// �ε巯�� ȸ���� ���� RInterpTo ���
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, NonPhysicsGrabRotationInterpSpeed);
			OwnerChar->SetActorRotation(NewRotation);
		}
	}

	// GrabVisualMesh ������Ʈ ���� (������ �����ϰ� �������� GetVisualCylinderStartLocation() ���)
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = NonPhysicsHitLocation;

		FVector VisualRopeDir = End - Start;
		float Length = VisualRopeDir.Size();
		// ... (���� GrabVisualMesh ���� ������ ���� �亯�� �����ϰ� ����) ...
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
			float ConstantThickness = 0.1f; // �β� �� (���� �ڵ忡�� 0.1f ���)
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
		FVector Start = GetVisualCylinderStartLocation(); // <--- ���⸦ ����! (Nozzle ���� ��ġ)
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

	FVector Start = GetVisualCylinderStartLocation(); // <--- ���⸦ ����! (Nozzle ���� ��ġ)
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
