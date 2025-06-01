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
	// 소켓 이름의 기본값을 설정합니다.
	CharacterMuzzleSocketName = TEXT("Buster"); // 여기에 실제 소켓 이름을 입력하세요.

	/* 캡슐 초기화 (기존 코드 유지) */
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* 이동 세팅 (기존 코드 유지) */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;		// 600은 인간 스프링임
	GetCharacterMovement()->AirControl = 0.3f;

	/* 카메라 (기존 코드 유지) */
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	CameraPivot->SetupAttachment(nullptr);
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraPivot);
	FollowCamera->bUsePawnControlRotation = false;

	/* 무기/입력 컴포넌트 (기존 코드 유지) */
	Shooter = CreateDefaultSubobject<UShooter>(TEXT("Shooter"));
	ElectricWeapon = CreateDefaultSubobject<UElectricWeapon>(TEXT("ElectricWeapon"));
	WaterWeapon = CreateDefaultSubobject<UWaterWeapon>(TEXT("WaterWeapon"));
	FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));
	InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));

	/* 화염방사 메시/콜라이더 (기존 코드 유지) */
	//여기서부터
	//FlameCylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameCylinderMesh"));
	//FlameCylinderMesh->SetupAttachment(RootComponent);
	//FlameCylinderMesh->SetHiddenInGame(true);
	//여기까지 캡슐
	FlameParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlameParticle"));
	FlameParticle->SetupAttachment(RootComponent);
	FlameParticle->SetAutoActivate(false);  // 초기엔 비활성화
	FlameParticle->bAutoActivate = false;
	FlameParticle->SetVisibility(false);    // 시각적으로도 숨김
	FlameCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FlameCollider"));
	FlameCollider->SetupAttachment(FlameParticle);
	FlameCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 초기엔 비활성
	FlameCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlameCollider->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	FlameCollider->SetGenerateOverlapEvents(true);
	FlameCollider->ComponentTags.Add(FName("Flame")); // 태그로 구분

	/* --- 체력 & 무적 초기화 --- */
	MaxHP = 6; // .h와 일치
	CurrentHP = MaxHP;
	bIsInvincible = false;
}

/* ---------- BeginPlay ---------- */
void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* --- HP/무적 초기화 --- */
	CurrentHP = MaxHP;
	bIsInvincible = false;
	HandleHPChange(); // UI 초기화를 위해 한번 호출
	OnInvincibilityChanged.Broadcast(false); // UI 초기화를 위해 한번 호출

	// 초기 무기 상태 UI 업데이트
	NotifyWeaponChanged();

	/* 마우스 설정 (기존 코드 유지) */
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);
	}

	/* 화염방사 비활성화 (기존 코드 유지) */
	if (FlameParticle)
	{
		FlameParticle->SetVisibility(false);       // 렌더링 숨김
		FlameParticle->DeactivateSystem();         // 파티클 비활성화
	}
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* 카메라 위치/회전 업데이트 (기존 코드 유지) */
	FVector CameraOffset = FVector(-1000.f, 0.f, 0.f); // 카메라 높이 0 -> 1000으로 수정
	FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f);
	FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
	CameraPivot->SetWorldLocation(NewLocation);
	CameraPivot->SetWorldRotation(CameraRotation);

	/* 회전 로직 (기존 코드 유지) */
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

	/* 입력 바인딩 (기존 코드 유지) */
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
			const float Speed = 100.f; // 기존 RInterpTo 유지
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
	GetWorld()->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
	EndInvincibility();
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

/* --- 기타 함수 구현 (기존 코드 유지) --- */
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
		FlameParticle->ActivateSystem(true);  // 루프 파티클 시작
	}
	else
	{
		FlameParticle->SetVisibility(false);
		FlameParticle->DeactivateSystem(); // 루프 파티클 중지
	}

	FlameCollider->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// StartFire 함수 구현 (만약 .h에 선언했다면, 내용은 비어있더라도 추가)
void APrototypeCharacter::StartFire()
{
	UE_LOG(LogTemp, Log, TEXT("StartFire called - Implement actual firing logic here or in weapons."));
}

FString APrototypeCharacter::GetCurrentWeaponName() const
{
	if (InputManager) // InputManager가 유효한지 확인
	{
		// InputManager의 새로운 public getter 함수들을 사용
		if (InputManager->IsElectricWeaponActive()) return TEXT("Electric Gun");
		if (InputManager->IsWaterWeaponActive())   return TEXT("Water Gun");
		if (InputManager->IsFlameWeaponActive())   return TEXT("Flame Gun");
	}
	return TEXT("Shooter"); // 기본값 또는 InputManager가 없을 경우
}


// 무기 변경 시 호출될 내부 함수 (새로 추가)
void APrototypeCharacter::NotifyWeaponChanged()
{
	CurrentWeaponDisplayName = GetCurrentWeaponName(); // 현재 무기 이름 업데이트
	OnWeaponChanged.Broadcast(CurrentWeaponDisplayName); // 델리게이트 호출
	UE_LOG(LogTemp, Warning, TEXT("APrototypeCharacter::NotifyWeaponChanged - Broadcasting OnWeaponChanged. NewWeapon: %s"), *CurrentWeaponDisplayName);
}