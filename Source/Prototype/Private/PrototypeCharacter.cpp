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

/* ---------- 생성자 ---------- */
APrototypeCharacter::APrototypeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CharacterMuzzleSocketName = TEXT("Buster");

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.3f;

	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	CameraPivot->SetupAttachment(RootComponent); // RootComponent에 붙이는 것이 일반적입니다. nullptr도 가능.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraPivot);
	FollowCamera->bUsePawnControlRotation = false;

	Shooter = CreateDefaultSubobject<UShooter>(TEXT("Shooter"));
	ElectricWeapon = CreateDefaultSubobject<UElectricWeapon>(TEXT("ElectricWeapon"));
	WaterWeapon = CreateDefaultSubobject<UWaterWeapon>(TEXT("WaterWeapon"));
	FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));
	InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));

	FlameParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlameParticle"));
	FlameParticle->SetupAttachment(RootComponent);
	FlameParticle->SetAutoActivate(false);
	FlameParticle->bAutoActivate = false;
	FlameParticle->SetVisibility(false);
	FlameCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FlameCollider"));
	FlameCollider->SetupAttachment(FlameParticle); // 파티클에 붙일 수도 있고, 캐릭터 루트나 특정 소켓에 붙일 수도 있습니다.
	FlameCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlameCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlameCollider->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	FlameCollider->SetGenerateOverlapEvents(true);
	FlameCollider->ComponentTags.Add(FName("Flame"));

	MaxHP = 6;
	CurrentHP = MaxHP;
	bIsInvincible = false;

	// 에너지 변수 초기화 (생성자에서 이미 0으로 초기화됨)
	basicEnergy = 0;
	flameEnergy = 0;
	waterEnergy = 0;
	electricEnergy = 0;
}

/* ---------- BeginPlay ---------- */
void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	bIsInvincible = false;
	HandleHPChange();
	OnInvincibilityChanged.Broadcast(false);

	NotifyWeaponChanged(); // 초기 무기 상태 UI 업데이트

	// 초기 에너지 상태 UI 업데이트를 위해 델리게이트 방송 (선택적, PlayerUIWidget의 InitializeForPlayer에서 직접 가져가도 됨)
	OnBasicEnergyChanged.Broadcast(basicEnergy);
	OnFlameEnergyChanged.Broadcast(flameEnergy);
	OnWaterEnergyChanged.Broadcast(waterEnergy);
	OnElectricEnergyChanged.Broadcast(electricEnergy);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);
	}

	if (FlameParticle)
	{
		FlameParticle->SetVisibility(false);
		FlameParticle->DeactivateSystem();
	}
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CameraOffset = FVector(-1000.f, 0.f, 0.f);
	FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f);
	if (GetMesh()) // 메시가 유효할 때 메시 위치를 기준으로 카메라 위치 계산
	{
		FVector NewLocation = GetMesh()->GetComponentLocation() + CameraRotation.RotateVector(CameraOffset);
		CameraPivot->SetWorldLocation(NewLocation);
		CameraPivot->SetWorldRotation(CameraRotation);
	}


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

/* ---------- 입력 바인딩 ---------- */
void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInput)
{
	check(PlayerInput);

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

/* ---------- 이동 & 점프 (기존 코드 유지) ---------- */
void APrototypeCharacter::MoveForward(float Value)
{
	if (Value == 0.f || !Controller) return; // Controller 유효성 검사 추가
	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Dir, Value);
}

void APrototypeCharacter::MoveRight(float Value)
{
	if (Value == 0.f || !Controller) return; // Controller 유효성 검사 추가
	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Dir, Value);
}

void APrototypeCharacter::Jump()
{
	Super::Jump();
}

/* ---------- 마우스 방향으로 회전 (기존 코드 유지) ---------- */
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
			const float Speed = 10.f; // RInterpTo 속도 조절 (기존 100.f는 매우 빠름)
			FRotator NewRot = FMath::RInterpTo(GetActorRotation(), RotationTarget, GetWorld()->GetDeltaSeconds(), Speed);
			SetActorRotation(NewRot);
		}
	}
}

/* --- 체력 (HP) 관련 함수 구현 --- */
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
	GetWorldTimerManager().ClearTimer(InvincibilityTimerHandle); // GetWorldTimerManager() 사용
	EndInvincibility(); // 죽었을 때 무적 상태 확실히 해제
	GetCharacterMovement()->DisableMovement();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) DisableInput(PC);
}

/* --- 무적 관련 함수 구현 --- */
void APrototypeCharacter::StartInvincibility()
{
	if (!bIsInvincible)
	{
		bIsInvincible = true;
		OnInvincibilityChanged.Broadcast(true);
		UE_LOG(LogTemp, Log, TEXT("Player Invincibility ON"));
		GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &APrototypeCharacter::EndInvincibility, InvincibilityDuration, false);
	}
}

void APrototypeCharacter::EndInvincibility()
{
	if (bIsInvincible) // 이미 무적이 해제된 경우 중복 호출 방지
	{
		bIsInvincible = false;
		OnInvincibilityChanged.Broadcast(false);
		UE_LOG(LogTemp, Log, TEXT("Player Invincibility OFF"));
	}
	GetWorldTimerManager().ClearTimer(InvincibilityTimerHandle); // 타이머 확실히 클리어
}

/* --- 에너지 획득 함수 구현 (델리게이트 방송 추가) --- */
void APrototypeCharacter::SetGetBasicEnergy(bool bValue)
{
	basicEnergy++;
	OnBasicEnergyChanged.Broadcast(basicEnergy); // 델리게이트 방송
	UE_LOG(LogTemp, Log, TEXT("Basic Energy changed: %d"), basicEnergy);
}

void APrototypeCharacter::SetGetFlameEnergy(bool bValue)
{
	flameEnergy++;
	OnFlameEnergyChanged.Broadcast(flameEnergy); // 델리게이트 방송
	UE_LOG(LogTemp, Log, TEXT("Flame Energy changed: %d"), flameEnergy);
}

void APrototypeCharacter::SetGetWaterEnergy(bool bValue)
{
	waterEnergy++;
	OnWaterEnergyChanged.Broadcast(waterEnergy); // 델리게이트 방송
	UE_LOG(LogTemp, Log, TEXT("Water Energy changed: %d"), waterEnergy);
}

void APrototypeCharacter::SetGetElectricEnergy(bool bValue) // 변수명 electricEnergy로 일치
{
	electricEnergy++;
	OnElectricEnergyChanged.Broadcast(electricEnergy); // 델리게이트 방송
	UE_LOG(LogTemp, Log, TEXT("Electric Energy changed: %d"), electricEnergy);
}

/* --- 기타 함수 구현 --- */
bool APrototypeCharacter::IsRotationLocked() const { return Shooter && Shooter->ShouldLockRotation(); }

void APrototypeCharacter::SetFlameVisible(bool bVisible)
{
	if (!FlameParticle || !FlameCollider) return;

	if (bVisible)
	{
		FlameParticle->SetVisibility(true);
		FlameParticle->ActivateSystem(true);
	}
	else
	{
		FlameParticle->SetVisibility(false);
		FlameParticle->DeactivateSystem();
	}
	FlameCollider->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void APrototypeCharacter::StartFire()
{
	UE_LOG(LogTemp, Log, TEXT("StartFire called - Implement actual firing logic here or in weapons."));
}

FString APrototypeCharacter::GetCurrentWeaponName() const
{
	if (InputManager)
	{
		if (InputManager->IsElectricWeaponActive()) return TEXT("Electric Gun");
		if (InputManager->IsWaterWeaponActive())   return TEXT("Water Gun");
		if (InputManager->IsFlameWeaponActive())   return TEXT("Flame Gun");
	}
	return TEXT("Shooter");
}

void APrototypeCharacter::NotifyWeaponChanged()
{
	CurrentWeaponDisplayName = GetCurrentWeaponName();
	OnWeaponChanged.Broadcast(CurrentWeaponDisplayName);
	UE_LOG(LogTemp, Warning, TEXT("APrototypeCharacter::NotifyWeaponChanged - Broadcasting OnWeaponChanged. NewWeapon: %s"), *CurrentWeaponDisplayName);
}