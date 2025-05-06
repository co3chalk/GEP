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

	// 마우스 위치를 월드 평면 위치로 변환
	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// 캐릭터와 같은 Z 높이의 평면과 마우스 방향이 교차하는 지점 구하기
		FPlane GroundPlane = FPlane(GetActorLocation(), FVector::UpVector); // XY 평면
		FVector LookAtTarget = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 10000.0f, GroundPlane);

		// 캐릭터 위치 기준으로 방향 계산
		FVector Direction = LookAtTarget - GetActorLocation();
		Direction.Z = 0.0f;

		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			FRotator CurrentRotation = GetActorRotation();

			float InterpSpeed = 10.0f; // 회전 속도 조절
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

	// 여기에서 직접 Static Mesh 로드
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
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 커서가 화면 밖으로 나갈 수 있음
		InputMode.SetHideCursorDuringCapture(false); // 마우스 클릭 시 커서 숨기지 않음
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

	// 라인트레이스 시작점: 캐릭터 위치에서 위로 약간 올림
	LineStart = GetActorLocation() + FVector(0, 0, 50.0f);
	FVector ForwardVector = GetActorForwardVector();
	LineEnd = LineStart + ForwardVector * GrabMaxDistance;

	// 자기 자신은 감지하지 않도록 예외 설정
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 물리 채널로 라인트레이스 수행
	bIsLineTraceHit = GetWorld()->LineTraceSingleByChannel(CachedHitResult, LineStart, LineEnd, ECC_PhysicsBody, Params);

	// 디버그 라인: 감지 성공 시 초록, 실패 시 빨간색
	FColor LineColor = bIsLineTraceHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, false, 0.0f, 0, 2.0f);

	// 물리 오브젝트를 잡고 있을 경우, 위치 지속 업데이트
	if (GrabbedComponent)
	{
		FVector TargetLocation = GetActorLocation() + ForwardVector * GrabbedObjectDistance;
		PhysicsHandle->InterpolationSpeed = 100.0f;
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// 줄 시각화 (GrabVisualMesh)
		if (GrabVisualMesh)
		{
			FVector Start = GetActorLocation();
			FVector End = TargetLocation;
			FVector RopeDir = End - Start;
			float Length = RopeDir.Size();
			FVector Mid = (Start + End) * 0.5f;

			// 카메라 방향과 줄 방향 각도 계산
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

	// 비물리 오브젝트에 매달려 있을 경우
	if (bIsGrabbingNonPhysics)
	{

		FVector Delta = GetActorLocation() - NonPhysicsHitLocation;

		// 축별 차이 로그 (문제 확인용)
		UE_LOG(LogTemp, Warning, TEXT("Delta X: %f, Y: %f, Z: %f"), Delta.X, Delta.Y, Delta.Z);

		GetCharacterMovement()->GravityScale = 1.0f;

		FVector RopeVector = Delta;
		float CurrentDistance = RopeVector.Size();
		FVector Direction = RopeVector.GetSafeNormal();
		FVector TargetLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
		// 로프 길이를 항상 유지 (강제 위치 수정 대신 속도로 보정)
		float RopeLength = NonPhysicsGrabDistance;
		FVector CurrentLocation = GetActorLocation();
		FVector IdealLocation = NonPhysicsHitLocation + Direction * RopeLength;
		FVector CorrectionVector = IdealLocation - CurrentLocation;

		// 캐릭터의 속도에 지속적으로 힘 누적 (진자 운동 효과)
		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		CurrentVelocity += CorrectionVector * CorrectionStrength * DeltaTime;

		// 입력 방향으로 추가적인 힘을 줘서 그네를 흔듦
		if (Controller)
		{
			FVector Forward = Controller->GetControlRotation().Vector();
			FVector Right = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);


			FVector InputVector =
				Forward * GetInputAxisValue("MoveForward") +
				Right * GetInputAxisValue("MoveRight");

			InputVector.Z = 0.0f; // Z축 영향 없음
			CurrentVelocity += InputVector * SwingForce * DeltaTime;
		}

		GetCharacterMovement()->Velocity = CurrentVelocity;
		// 카메라 방향과 줄 방향 각도 계산
		if (GrabVisualMesh)
		{
			FVector CameraDir = FollowCamera->GetForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraDir, RopeVector.GetSafeNormal())));
			float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;
			float ScaleZ = RopeVector.Size() / 100.0f;
			FVector Mid = (GetActorLocation() + NonPhysicsHitLocation) * 0.5f;
			float ConstantThickness = 0.5f; // 일정한 두께 설정 (고정값)

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeVector).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}

		// 우클릭으로 줄 당기기
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

		// 결계 유지 (구형 제한 방식으로 변경한 부분)
		float MaxAllowedDistance = NonPhysicsGrabDistance + 0.0f;  // 최대 거리 설정값
		FVector DesiredLocation = GetActorLocation();

		FVector OffsetFromCenter = DesiredLocation - NonPhysicsHitLocation;
		float OffsetLength = OffsetFromCenter.Size();

		if (OffsetLength > MaxAllowedDistance)
		{
			OffsetFromCenter = OffsetFromCenter.GetSafeNormal() * MaxAllowedDistance;
			FVector CorrectedLocation = NonPhysicsHitLocation + OffsetFromCenter;

			SetActorLocation(CorrectedLocation, false);
		}

		// 디버그 시각화 (유지 가능)
		DrawDebugLine(GetWorld(), GetActorLocation(), NonPhysicsHitLocation, FColor::Purple, false, 0.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, 15.0f, 12, FColor::Yellow, false, 0.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, MaxAllowedDistance, 32, FColor::Green, false, 0.0f, 0, 0.5f);
	}


	// 아무것도 잡지 못했을 때 Grab 시 실린더 일시적으로 표시
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
			CurrentVelocity.X = 0.0f;  // Z축 힘만 초기화
			CurrentVelocity.Y = 0.0f;  // Z축 힘만 초기화
			GetCharacterMovement()->Velocity = CurrentVelocity;
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor(128, 0, 128), false, 1.0f, 0, 2.0f);
			NonPhysicsHitLocation = CachedHitResult.ImpactPoint;
			NonPhysicsGrabDistance = FVector::Dist(GetActorLocation(), NonPhysicsHitLocation);
			bIsGrabbingNonPhysics = true;
		}
	}

	// 시각화 메쉬는 항상 표시 (잡든 못 잡든)
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
		CurrentVelocity.Z = 0.0f;  // Z축 힘만 초기화

	}
	bIsGrabbingNonPhysics = false;
	GetCharacterMovement()->GravityScale = 1.0f;
	if (GrabVisualMesh)
	{
		GrabVisualMesh->SetVisibility(false);
	}

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bNonPhysicsCollisionOccurred = false; // 충돌 리셋

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

		// 실린더 업데이트
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

		// 실린더 업데이트
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