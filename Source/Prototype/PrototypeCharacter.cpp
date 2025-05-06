#include "PrototypeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

void APrototypeCharacter::RotateCharacterToMouse()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !FollowCamera) return;

	// ���콺 ��ġ�� ���� ��� ��ġ�� ��ȯ
	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// ĳ���Ϳ� ���� Z ������ ���� ���콺 ������ �����ϴ� ���� ���ϱ�
		FPlane GroundPlane = FPlane(GetActorLocation(), FVector::UpVector); // XY ���
		FVector LookAtTarget = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 10000.0f, GroundPlane);

		// ĳ���� ��ġ �������� ���� ���
		FVector Direction = LookAtTarget - GetActorLocation();
		Direction.Z = 0.0f;

		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			FRotator CurrentRotation = GetActorRotation();

			float InterpSpeed = 10.0f; // ȸ�� �ӵ� ����
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
			SetActorRotation(NewRotation);
		}
	}
}


APrototypeCharacter::APrototypeCharacter()
{
	GrabVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrabVisualMesh"));
	GrabVisualMesh->SetupAttachment(RootComponent);
	GrabVisualMesh->SetVisibility(false);
	GrabVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ���⿡�� ���� Static Mesh �ε�
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/SM_GrabCylinder.SM_GrabCylinder"));
	if (MeshAsset.Succeeded())
	{
		GrabVisualMesh->SetStaticMesh(MeshAsset.Object);
		UE_LOG(LogTemp, Error, TEXT("O GrabVisualMesh Static Mesh"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("X GrabVisualMesh Static Mesh"));
	}


	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.3f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	RotationConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RotationConstraint"));
	RotationConstraint->SetupAttachment(RootComponent);

	GrabMaxDistance = 700.f;
	GrabMinDistance = 200.f;
	GrabbedComponent = nullptr;
	GrabbedObjectDistance = 0.0f;
	ScrollDistanceSpeed = 50.0f;

	bIsLineTraceHit = false;
	bIsGrabbingNonPhysics = false;
	bIsRightMouseButtonDown = false;

	NonPhysicsDistanceReduceSpeed = 1500.0f;
}

void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;


		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // Ŀ���� ȭ�� ������ ���� �� ����
		InputMode.SetHideCursorDuringCapture(false); // ���콺 Ŭ�� �� Ŀ�� ������ ����
		PC->SetInputMode(InputMode);
	}
}
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(GrabbedComponent && GrabbedComponent->IsSimulatingPhysics()))
	{
		RotateCharacterToMouse();
	}

	// ����Ʈ���̽� ������: ĳ���� ��ġ���� ���� �ణ �ø�
	LineStart = GetActorLocation() + FVector(0, 0, 50.0f);
	FVector ForwardVector = GetActorForwardVector();
	LineEnd = LineStart + ForwardVector * GrabMaxDistance;

	// �ڱ� �ڽ��� �������� �ʵ��� ���� ����
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// ���� ä�η� ����Ʈ���̽� ����
	bIsLineTraceHit = GetWorld()->LineTraceSingleByChannel(CachedHitResult, LineStart, LineEnd, ECC_PhysicsBody, Params);

	// ����� ����: ���� ���� �� �ʷ�, ���� �� ������
	FColor LineColor = bIsLineTraceHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, false, 0.0f, 0, 2.0f);

	// ���� ������Ʈ�� ��� ���� ���, ��ġ ���� ������Ʈ
	if (GrabbedComponent)
	{
		FVector TargetLocation = GetActorLocation() + ForwardVector * GrabbedObjectDistance;
		PhysicsHandle->InterpolationSpeed = 100.0f;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// �� �ð�ȭ (GrabVisualMesh)
		if (GrabVisualMesh)
		{
			FVector Start = GetActorLocation();
			FVector End = TargetLocation;
			FVector RopeDir = End - Start;
			float Length = RopeDir.Size();
			FVector Mid = (Start + End) * 0.5f;

			// ī�޶� ����� �� ���� ���� ���
			FVector CameraDir = FollowCamera->GetForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
			float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
		}
	}

	// �񹰸� ������Ʈ�� �Ŵ޷� ���� ���
	if (bIsGrabbingNonPhysics)
	{

		FVector Delta = GetActorLocation() - NonPhysicsHitLocation;

		// �ະ ���� �α� (���� Ȯ�ο�)
		UE_LOG(LogTemp, Warning, TEXT("Delta X: %f, Y: %f, Z: %f"), Delta.X, Delta.Y, Delta.Z);

		GetCharacterMovement()->GravityScale = 1.0f;

		FVector RopeVector = Delta;
		float CurrentDistance = RopeVector.Size();
		FVector Direction = RopeVector.GetSafeNormal();
		FVector TargetLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
		// ���� ���̸� �׻� ���� (���� ��ġ ���� ��� �ӵ��� ����)
		float RopeLength = NonPhysicsGrabDistance;
		FVector CurrentLocation = GetActorLocation();
		FVector IdealLocation = NonPhysicsHitLocation + Direction * RopeLength;
		FVector CorrectionVector = IdealLocation - CurrentLocation;

		// ĳ������ �ӵ��� ���������� �� ���� (���� � ȿ��)
		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		CurrentVelocity += CorrectionVector * CorrectionStrength * DeltaTime;

		// �Է� �������� �߰����� ���� �༭ �׳׸� ���
		if (Controller)
		{
			FVector Forward = Controller->GetControlRotation().Vector();
			FVector Right = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);


			FVector InputVector =
				Forward * GetInputAxisValue("MoveForward") +
				Right * GetInputAxisValue("MoveRight");

			InputVector.Z = 0.0f; // Z�� ���� ����
			CurrentVelocity += InputVector * SwingForce * DeltaTime;
		}

		GetCharacterMovement()->Velocity = CurrentVelocity;
		// ī�޶� ����� �� ���� ���� ���
		if (GrabVisualMesh)
		{
			FVector CameraDir = FollowCamera->GetForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraDir, RopeVector.GetSafeNormal())));
			float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;
			float ScaleZ = RopeVector.Size() / 100.0f;
			FVector Mid = (GetActorLocation() + NonPhysicsHitLocation) * 0.5f;
			float ConstantThickness = 0.5f; // ������ �β� ���� (������)

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeVector).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}

		// ��Ŭ������ �� ����
		if (bIsRightMouseButtonDown)
		{
			NonPhysicsGrabDistance -= NonPhysicsDistanceReduceSpeed * DeltaTime;
			NonPhysicsGrabDistance = FMath::Max(NonPhysicsGrabDistance, 100.0f);

			FVector PullTarget = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
			FVector NewLocation = FMath::VInterpTo(GetActorLocation(), PullTarget, DeltaTime, 800.0f);
			SetActorLocation(NewLocation, true);
		}
		else
		{
			FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 100.0f);
			SetActorLocation(NewLocation, true);
		}

		// ��� ���� (���� ���� ������� ������ �κ�)
		float MaxAllowedDistance = NonPhysicsGrabDistance + 0.0f;  // �ִ� �Ÿ� ������
		FVector DesiredLocation = GetActorLocation();

		FVector OffsetFromCenter = DesiredLocation - NonPhysicsHitLocation;
		float OffsetLength = OffsetFromCenter.Size();

		if (OffsetLength > MaxAllowedDistance)
		{
			OffsetFromCenter = OffsetFromCenter.GetSafeNormal() * MaxAllowedDistance;
			FVector CorrectedLocation = NonPhysicsHitLocation + OffsetFromCenter;

			SetActorLocation(CorrectedLocation, false);
		}

		// ����� �ð�ȭ (���� ����)
		DrawDebugLine(GetWorld(), GetActorLocation(), NonPhysicsHitLocation, FColor::Purple, false, 0.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, 15.0f, 12, FColor::Yellow, false, 0.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, MaxAllowedDistance, 32, FColor::Green, false, 0.0f, 0, 0.5f);
	}


	// �ƹ��͵� ���� ������ �� Grab �� �Ǹ��� �Ͻ������� ǥ��
	if (bShowMissedGrabVisual && GrabVisualMesh)
	{
		MissedGrabTimer -= DeltaTime;

		if (MissedGrabTimer <= 0.0f)
		{
			bShowMissedGrabVisual = false;
			GrabVisualMesh->SetVisibility(false);
		}
		else
		{
			FVector Start = GetActorLocation();
			FVector End = MissedGrabTarget;
			FVector RopeDir = End - Start;
			float Length = RopeDir.Size();
			FVector Mid = (Start + End) * 0.5f;

			FVector CameraDir = FollowCamera->GetForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
			float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
		}
	}
}




void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APrototypeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APrototypeCharacter::MoveRight);

	PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &APrototypeCharacter::Grab);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &APrototypeCharacter::Release);

	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &APrototypeCharacter::ScrollUpFunction);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &APrototypeCharacter::ScrollDownFunction);

	PlayerInputComponent->BindAction("RightMouseButton", IE_Pressed, this, &APrototypeCharacter::RightMouseDown);
	PlayerInputComponent->BindAction("RightMouseButton", IE_Released, this, &APrototypeCharacter::RightMouseUp);
}

void APrototypeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{


		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APrototypeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{


		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APrototypeCharacter::Jump()
{


	Super::Jump();
}


void APrototypeCharacter::Grab()
{
	if (bIsLineTraceHit)
	{
		UPrimitiveComponent* HitComponent = CachedHitResult.GetComponent();
		if (!HitComponent) return;

		if (HitComponent->IsSimulatingPhysics())
		{
			float CurrentDistance = FVector::Dist(GetActorLocation(), HitComponent->GetComponentLocation());
			if (CurrentDistance < GrabMinDistance || CurrentDistance > GrabMaxDistance) return;

			HitComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			HitComponent->SetEnableGravity(false);

			GrabbedObjectDistance = CurrentDistance;
			GrabbedComponent = HitComponent;

			PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetComponentLocation());
			PhysicsHandle->InterpolationSpeed = 100.0f;
			PhysicsHandle->SetTargetLocation(GetActorLocation() + GetActorForwardVector() * GrabbedObjectDistance);

			RotationConstraint->SetWorldLocation(HitComponent->GetComponentLocation());
			RotationConstraint->SetConstrainedComponents(nullptr, NAME_None, HitComponent, NAME_None);
			RotationConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);

			DrawDebugSphere(GetWorld(), HitComponent->GetComponentLocation(), 20.f, 12, FColor::Yellow, false, 1.0f);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			FVector CurrentVelocity = GetCharacterMovement()->Velocity;
			CurrentVelocity.X = 0.0f;  // Z�� ���� �ʱ�ȭ
			CurrentVelocity.Y = 0.0f;  // Z�� ���� �ʱ�ȭ
			GetCharacterMovement()->Velocity = CurrentVelocity;
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor(128, 0, 128), false, 1.0f, 0, 2.0f);
			NonPhysicsHitLocation = CachedHitResult.ImpactPoint;
			NonPhysicsGrabDistance = FVector::Dist(GetActorLocation(), NonPhysicsHitLocation);
			bIsGrabbingNonPhysics = true;
		}
	}

	// �ð�ȭ �޽��� �׻� ǥ�� (��� �� ���)
	if (!GrabbedComponent && !bIsGrabbingNonPhysics)
	{
		MissedGrabTarget = LineEnd;
		MissedGrabTimer = MissedGrabDuration;
		bShowMissedGrabVisual = true;

		if (GrabVisualMesh)
		{
			GrabVisualMesh->SetVisibility(true);
		}
	}
}

void APrototypeCharacter::Release()
{
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
		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = 0.0f;  // Z�� ���� �ʱ�ȭ

	}
	bIsGrabbingNonPhysics = false;
	GetCharacterMovement()->GravityScale = 1.0f;
	if (GrabVisualMesh)
	{
		GrabVisualMesh->SetVisibility(false);
	}

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bNonPhysicsCollisionOccurred = false; // �浹 ����

}
void APrototypeCharacter::UpdateGrabVisualMesh()
{
	if (!GrabVisualMesh || !GrabbedComponent) return;

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * GrabbedObjectDistance;
	FVector RopeDir = End - Start;
	float Length = RopeDir.Size();
	FVector Mid = (Start + End) * 0.5f;
	float ScaleZ = Length / 100.0f;
	FRotator Rotation = FRotationMatrix::MakeFromZ(RopeDir).Rotator();


	FVector CameraDir = FollowCamera->GetForwardVector();

	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
	float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

	GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, Length / 100.0f));

}

void APrototypeCharacter::ScrollUpFunction()
{
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance + ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// �Ǹ��� ������Ʈ
		UpdateGrabVisualMesh();
	}
}

void APrototypeCharacter::ScrollDownFunction()
{
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance - ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// �Ǹ��� ������Ʈ
		UpdateGrabVisualMesh();
	}
}


void APrototypeCharacter::RightMouseDown()
{
	bIsRightMouseButtonDown = true;
}

void APrototypeCharacter::RightMouseUp()
{
	bIsRightMouseButtonDown = false;
}