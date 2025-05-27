#include "PrototypeCharacter.h"
#include "Shooter.h"
#include "ElectricWeapon.h"
#include "WaterWeapon.h"
#include "FlameWeapon.h"
#include "InputManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"


/* ---------- 생성자 ---------- */
APrototypeCharacter::APrototypeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	/* 캡슐 초기화 (원본과 동일) */
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* 이동 세팅 */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.3f;

	// 카메라
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	CameraPivot->SetupAttachment(nullptr); // 캐릭터에 부착하지 않음


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraPivot);
	FollowCamera->bUsePawnControlRotation = false;


	/* 발사체 관련 컴포넌트 생성 */
	Shooter = CreateDefaultSubobject<UShooter>(TEXT("Shooter"));
	ElectricWeapon = CreateDefaultSubobject<UElectricWeapon>(TEXT("ElectricWeapon"));
	WaterWeapon = CreateDefaultSubobject<UWaterWeapon>(TEXT("WaterWeapon"));
	FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));

	/* 화염방사 관련 메시 컴포넌트 생성 */
	FlameCylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameCylinderMesh"));
	FlameCylinderMesh->SetupAttachment(RootComponent); // 루트에 붙이거나 원하는 본에 붙이기
	FlameCylinderCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CylinderCollider"));
	FlameCylinderCollider->SetupAttachment(FlameCylinderMesh);

	FlameCylinderMesh->SetHiddenInGame(true);	// 처음에 꺼진 상태로 설정
	FlameCylinderCollider->SetHiddenInGame(true);
	FlameCylinderCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*인풋 매니저 생성*/
	InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));
}

/* ---------- BeginPlay ---------- */
void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{ 
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);
	}

	/* --- 화염방사 비활성화 --- */
	if (FlameCylinderMesh)
		FlameCylinderMesh->SetHiddenInGame(true);

	if (FlameCylinderCollider)
	{
		FlameCylinderCollider->SetHiddenInGame(true);
		FlameCylinderCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CameraOffset = FVector(-1000.f, 0.f, 0000.f); // 캐릭터 뒤 + 위쪽
	FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f); // 위에서 내려다보는 각도

	FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);

	CameraPivot->SetWorldLocation(NewLocation);
	CameraPivot->SetWorldRotation(CameraRotation);
	if (IsRotationLocked())
		return;

	if (bShouldRotateToMouse)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		RotateCharacterToMouse();

		// 회전 완료 확인
		if (GetActorRotation().Equals(RotationTarget, 1.0f))
		{
			bShouldRotateToMouse = false;

			// 예약된 동작이 있으면 실행
			if (bWaitingForPostRotationAction && PostRotationAction)
			{
				bWaitingForPostRotationAction = false;
				PostRotationAction();
				PostRotationAction = nullptr;
			}
		}
	}
	else
	{
		// 기본은 이동 방향 회전
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}



/* ---------- 입력 바인딩 ---------- */
void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInput)
{
	check(PlayerInput);

	PlayerInput->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInput->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInput->BindAxis("MoveForward", this, &APrototypeCharacter::MoveForward);
	PlayerInput->BindAxis("MoveRight", this, &APrototypeCharacter::MoveRight);

	/* Grab / Scroll / 우클릭 → 인풋매니저 로 위임 */
	PlayerInput->BindAction("LeftMouseButton", IE_Pressed, InputManager, &UInputManager::HandleGrab);
	PlayerInput->BindAction("LeftMouseButton", IE_Released, InputManager, &UInputManager::HandleRelease);

	PlayerInput->BindAction("ScrollUp", IE_Pressed, InputManager, &UInputManager::HandleScrollUp);
	PlayerInput->BindAction("ScrollDown", IE_Pressed, InputManager, &UInputManager::HandleScrollDown);

	PlayerInput->BindAction("RightMouseButton", IE_Pressed, InputManager, &UInputManager::HandleRightMouseDown);
	PlayerInput->BindAction("RightMouseButton", IE_Released, InputManager, &UInputManager::HandleRightMouseUp);

	PlayerInput->BindAction("SwapWeapon", IE_Pressed, InputManager, &UInputManager::HandleSwapWeapon);
}

/* ---------- 이동 ---------- */
void APrototypeCharacter::MoveForward(float Value)
{
	if (Value == 0.f) return;

	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Dir, Value);
}

void APrototypeCharacter::MoveRight(float Value)
{
	if (Value == 0.f) return;

	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Dir, Value);
}

void APrototypeCharacter::Jump()
{
	Super::Jump();
}

/* ---------- 마우스 방향으로 회전 ---------- */
void APrototypeCharacter::RotateCharacterToMouse()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !FollowCamera) return;

	FVector WorldLoc, WorldDir;
	if (PC->DeprojectMousePositionToWorld(WorldLoc, WorldDir))
	{
		FPlane GroundPlane(GetActorLocation(), FVector::UpVector);
		FVector Target = FMath::LinePlaneIntersection(WorldLoc, WorldLoc + WorldDir * 10000.f, GroundPlane);
		FVector Dir = Target - GetActorLocation();
		Dir.Z = 0.f;

		if (!Dir.IsNearlyZero())
		{
			// 목표 회전 저장
			RotationTarget = Dir.Rotation();

			// 부드럽게 보간 회전
			const float Speed = 100.f;
			FRotator NewRot = FMath::RInterpTo(GetActorRotation(), RotationTarget, GetWorld()->GetDeltaSeconds(), Speed);
			SetActorRotation(NewRot);
		}
	}
}



bool APrototypeCharacter::IsRotationLocked() const
{
	return Shooter && Shooter->ShouldLockRotation();
}

void APrototypeCharacter::SetGetEnergy(bool bValue)
{
	bGetEnergy = bValue;
}
void APrototypeCharacter::SetGetFlameEnergy(bool bValue)
{
	bGetFlameEnergy = bValue;
}
void APrototypeCharacter::SetGetWaterEnergy(bool bValue)
{
	bGetWaterEnergy = bValue;
}
void APrototypeCharacter::SetGetElectricEnergy(bool bValue)
{
	bGetElectricEnergy = bValue;
}

void APrototypeCharacter::SetFlameCylinderVisible(bool bVisible)
{
	if (!FlameCylinderMesh || !FlameCylinderCollider) return;

	FlameCylinderMesh->SetHiddenInGame(!bVisible);
	FlameCylinderCollider->SetHiddenInGame(!bVisible);
	FlameCylinderCollider->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
