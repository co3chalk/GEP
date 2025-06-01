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
#include "Engine/World.h" 
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"



/* ---------- ������ ---------- */
APrototypeCharacter::APrototypeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// ���� �̸��� �⺻���� �����մϴ�.
	CharacterMuzzleSocketName = TEXT("Buster"); // ���⿡ ���� ���� �̸��� �Է��ϼ���.

	/* ĸ�� �ʱ�ȭ (���� �ڵ� ����) */
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* �̵� ���� (���� �ڵ� ����) */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;		// 600�� �ΰ� ��������
	GetCharacterMovement()->AirControl = 0.3f;

	/* ī�޶� (���� �ڵ� ����) */
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	CameraPivot->SetupAttachment(nullptr);
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraPivot);
	FollowCamera->bUsePawnControlRotation = false;

	/* ����/�Է� ������Ʈ (���� �ڵ� ����) */
	Shooter = CreateDefaultSubobject<UShooter>(TEXT("Shooter"));
	ElectricWeapon = CreateDefaultSubobject<UElectricWeapon>(TEXT("ElectricWeapon"));
	WaterWeapon = CreateDefaultSubobject<UWaterWeapon>(TEXT("WaterWeapon"));
	FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));
	InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));

	/* ȭ����� �޽�/�ݶ��̴� (���� �ڵ� ����) */
	//���⼭����
	//FlameCylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameCylinderMesh"));
	//FlameCylinderMesh->SetupAttachment(RootComponent);
	//FlameCylinderMesh->SetHiddenInGame(true);
	//������� ĸ��
	FlameParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlameParticle"));
	FlameParticle->SetupAttachment(RootComponent);
	FlameParticle->SetAutoActivate(false);  // �ʱ⿣ ��Ȱ��ȭ
	FlameParticle->bAutoActivate = false;
	FlameParticle->SetVisibility(false);    // �ð������ε� ����
	FlameCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FlameCollider"));
	FlameCollider->SetupAttachment(FlameParticle);
	FlameCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �ʱ⿣ ��Ȱ��
	FlameCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlameCollider->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	FlameCollider->SetGenerateOverlapEvents(true);
	FlameCollider->ComponentTags.Add(FName("Flame")); // �±׷� ����

	/* --- ü�� & ���� �ʱ�ȭ --- */
	MaxHP = 6; // .h�� ��ġ
	CurrentHP = MaxHP;
	bIsInvincible = false;
}

/* ---------- BeginPlay ---------- */
void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* --- HP/���� �ʱ�ȭ --- */
	CurrentHP = MaxHP;
	bIsInvincible = false;
	HandleHPChange(); // UI �ʱ�ȭ�� ���� �ѹ� ȣ��
	OnInvincibilityChanged.Broadcast(false); // UI �ʱ�ȭ�� ���� �ѹ� ȣ��

	// �ʱ� ���� ���� UI ������Ʈ
	NotifyWeaponChanged();

	/* ���콺 ���� (���� �ڵ� ����) */
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);
	}

	/* ȭ����� ��Ȱ��ȭ (���� �ڵ� ����) */
	if (FlameParticle)
	{
		FlameParticle->SetVisibility(false);       // ������ ����
		FlameParticle->DeactivateSystem();         // ��ƼŬ ��Ȱ��ȭ
	}
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* ī�޶� ��ġ/ȸ�� ������Ʈ (���� �ڵ� ����) */
	FVector CameraOffset = FVector(-1000.f, 0.f, 0.f); // ī�޶� ���� 0 -> 1000���� ����
	FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f);
	FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
	CameraPivot->SetWorldLocation(NewLocation);
	CameraPivot->SetWorldRotation(CameraRotation);

	/* ȸ�� ���� (���� �ڵ� ����) */
	if (IsRotationLocked())
		return;

	if (bShouldRotateToMouse)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		RotateCharacterToMouse();

		if (GetActorRotation().Equals(RotationTarget, 1.0f))
		{
			bShouldRotateToMouse = false;
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
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

/* ---------- �Է� ���ε� ---------- */
void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInput)
{
	check(PlayerInput);

	/* �Է� ���ε� (���� �ڵ� ����) */
	PlayerInput->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInput->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInput->BindAxis("MoveForward", this, &APrototypeCharacter::MoveForward);
	PlayerInput->BindAxis("MoveRight", this, &APrototypeCharacter::MoveRight);
	PlayerInput->BindAction("LeftMouseButton", IE_Pressed, InputManager, &UInputManager::HandleGrab);
	PlayerInput->BindAction("LeftMouseButton", IE_Released, InputManager, &UInputManager::HandleRelease);
	PlayerInput->BindAction("ScrollUp", IE_Pressed, InputManager, &UInputManager::HandleScrollUp);
	PlayerInput->BindAction("ScrollDown", IE_Pressed, InputManager, &UInputManager::HandleScrollDown);
	PlayerInput->BindAction("RightMouseButton", IE_Pressed, InputManager, &UInputManager::HandleRightMouseDown);
	PlayerInput->BindAction("RightMouseButton", IE_Released, InputManager, &UInputManager::HandleRightMouseUp);
	PlayerInput->BindAction("SwapWeapon", IE_Pressed, InputManager, &UInputManager::HandleSwapWeapon);
}

/* ---------- �̵� & ���� (���� �ڵ� ����) ---------- */
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

/* ---------- ���콺 �������� ȸ�� (���� �ڵ� ����) ---------- */
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
			RotationTarget = Dir.Rotation();
			const float Speed = 100.f; // ���� RInterpTo ����
			FRotator NewRot = FMath::RInterpTo(GetActorRotation(), RotationTarget, GetWorld()->GetDeltaSeconds(), Speed);
			SetActorRotation(NewRot);
		}
	}
}

/* --- ü�� (HP) ���� �Լ� ���� --- */
int32 APrototypeCharacter::GetMaxHP() const { return MaxHP; }
int32 APrototypeCharacter::GetCurrentHP() const { return CurrentHP; }
bool APrototypeCharacter::IsInvincible() const { return bIsInvincible; }

void APrototypeCharacter::TakeDamage(int32 DamageAmount)
{
	if (bIsInvincible || DamageAmount <= 0 || CurrentHP <= 0) return;

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("Player took %d damage, Current HP: %d"), DamageAmount, CurrentHP);
	HandleHPChange();
	if (CurrentHP > 0) StartInvincibility();
}

void APrototypeCharacter::HandleHPChange()
{
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
	if (CurrentHP <= 0) Die();
}

void APrototypeCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died!"));
	GetWorld()->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
	EndInvincibility();
	GetCharacterMovement()->DisableMovement();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) DisableInput(PC);
}

/* --- ���� ���� �Լ� ���� --- */
void APrototypeCharacter::StartInvincibility()
{
	if (!bIsInvincible)
	{
		bIsInvincible = true;
		OnInvincibilityChanged.Broadcast(true);
		UE_LOG(LogTemp, Log, TEXT("Player Invincibility ON"));
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &APrototypeCharacter::EndInvincibility, InvincibilityDuration, false);
	}
}

void APrototypeCharacter::EndInvincibility()
{
	if (bIsInvincible)
	{
		bIsInvincible = false;
		OnInvincibilityChanged.Broadcast(false);
		UE_LOG(LogTemp, Log, TEXT("Player Invincibility OFF"));
	}
	GetWorld()->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
}

/* --- ��Ÿ �Լ� ���� (���� �ڵ� ����) --- */
bool APrototypeCharacter::IsRotationLocked() const { return Shooter && Shooter->ShouldLockRotation(); }
void APrototypeCharacter::SetGetBasicEnergy(bool bValue) { basicEnergy++; }
void APrototypeCharacter::SetGetFlameEnergy(bool bValue) { flameEnergy++; }
void APrototypeCharacter::SetGetWaterEnergy(bool bValue) { waterEnergy++; }
void APrototypeCharacter::SetGetElectricEnergy(bool bValue) { electricEnergy++; }

void APrototypeCharacter::SetFlameVisible(bool bVisible)
{
	if (!FlameParticle || !FlameCollider) return;

	if (bVisible)
	{
		FlameParticle->SetVisibility(true);
		FlameParticle->ActivateSystem(true);  // ���� ��ƼŬ ����
	}
	else
	{
		FlameParticle->SetVisibility(false);
		FlameParticle->DeactivateSystem(); // ���� ��ƼŬ ����
	}

	FlameCollider->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// StartFire �Լ� ���� (���� .h�� �����ߴٸ�, ������ ����ִ��� �߰�)
void APrototypeCharacter::StartFire()
{
	UE_LOG(LogTemp, Log, TEXT("StartFire called - Implement actual firing logic here or in weapons."));
}

FString APrototypeCharacter::GetCurrentWeaponName() const
{
	if (InputManager) // InputManager�� ��ȿ���� Ȯ��
	{
		// InputManager�� ���ο� public getter �Լ����� ���
		if (InputManager->IsElectricWeaponActive()) return TEXT("Electric Gun");
		if (InputManager->IsWaterWeaponActive())   return TEXT("Water Gun");
		if (InputManager->IsFlameWeaponActive())   return TEXT("Flame Gun");
	}
	return TEXT("Shooter"); // �⺻�� �Ǵ� InputManager�� ���� ���
}


// ���� ���� �� ȣ��� ���� �Լ� (���� �߰�)
void APrototypeCharacter::NotifyWeaponChanged()
{
	CurrentWeaponDisplayName = GetCurrentWeaponName(); // ���� ���� �̸� ������Ʈ
	OnWeaponChanged.Broadcast(CurrentWeaponDisplayName); // ��������Ʈ ȣ��
	UE_LOG(LogTemp, Warning, TEXT("APrototypeCharacter::NotifyWeaponChanged - Broadcasting OnWeaponChanged. NewWeapon: %s"), *CurrentWeaponDisplayName);
}