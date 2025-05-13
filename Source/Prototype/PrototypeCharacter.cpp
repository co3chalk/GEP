#include "PrototypeCharacter.h"
#include "Shooter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

/* ---------- ������ ---------- */
APrototypeCharacter::APrototypeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	/* ĸ�� �ʱ�ȭ (������ ����) */
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* �̵� ���� */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.3f;

	/* ī�޶� �� */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	/* �ȷο� ī�޶� */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	/* Shooter ������Ʈ ���� */
	Shooter = CreateDefaultSubobject<UShooter>(TEXT("Shooter"));
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
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* Shooter�� ��ü�� ��� ���� ���� ���� ȸ�� */
	if (!IsRotationLocked())
	{
		RotateCharacterToMouse();
	}
}

/* ---------- �Է� ���ε� ---------- */
void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInput)
{
	check(PlayerInput);

	PlayerInput->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInput->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInput->BindAxis("MoveForward", this, &APrototypeCharacter::MoveForward);
	PlayerInput->BindAxis("MoveRight", this, &APrototypeCharacter::MoveRight);

	/* Grab / Scroll / ��Ŭ�� �� Shooter �� ���� */
	PlayerInput->BindAction("LeftMouseButton", IE_Pressed, Shooter, &UShooter::Grab);
	PlayerInput->BindAction("LeftMouseButton", IE_Released, Shooter, &UShooter::Release);

	PlayerInput->BindAction("ScrollUp", IE_Pressed, Shooter, &UShooter::ScrollUp);
	PlayerInput->BindAction("ScrollDown", IE_Pressed, Shooter, &UShooter::ScrollDown);

	PlayerInput->BindAction("RightMouseButton", IE_Pressed, Shooter, &UShooter::RightMouseDown);
	PlayerInput->BindAction("RightMouseButton", IE_Released, Shooter, &UShooter::RightMouseUp);
}

/* ---------- �̵� ---------- */
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

/* ---------- ���콺 �������� ȸ�� ---------- */
void APrototypeCharacter::RotateCharacterToMouse()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !FollowCamera) return;

	FVector WorldLoc, WorldDir;
	if (PC->DeprojectMousePositionToWorld(WorldLoc, WorldDir))
	{
		FPlane   GroundPlane(GetActorLocation(), FVector::UpVector);
		FVector  Target = FMath::LinePlaneIntersection(WorldLoc,
			WorldLoc + WorldDir * 10000.f,
			GroundPlane);
		FVector  Dir = Target - GetActorLocation();
		Dir.Z = 0.f;

		if (!Dir.IsNearlyZero())
		{
			const FRotator TargetRot = Dir.Rotation();
			const float    Speed = 10.f;
			FRotator NewRot = FMath::RInterpTo(GetActorRotation(),
				TargetRot,
				GetWorld()->GetDeltaSeconds(),
				Speed);
			SetActorRotation(NewRot);
		}
	}
}

bool APrototypeCharacter::IsRotationLocked() const
{
	return Shooter && Shooter->ShouldLockRotation();
}
