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
#include "Components/CapsuleComponent.h"     // GetCapsuleComponent() 사용
#include "Kismet/GameplayStatics.h"      // GetGameInstance, GetGameMode 등 사용
#include "PrototypeGameInstance.h"     // UPrototypeGameInstance 클래스 사용
#include "PrototypeGameMode.h"             // APrototypeGameMode 클래스 사용



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
    GetCharacterMovement()->JumpZVelocity = 500.f;      // 600은 인간 스프링임
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
    FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));
    InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));

    /* 화염방사 메시/콜라이더 (기존 코드 유지) */
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

    CurrentHP = MaxHP;

}

/* ---------- BeginPlay ---------- */
void APrototypeCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (!Shooter)
    {
        Shooter = FindComponentByClass<UShooter>();
        if (Shooter)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character::BeginPlay - Shooter component was NULL, but successfully found and re-linked!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Character::BeginPlay - FATAL ERROR: Could not find Shooter component at all!"));
        }
    }

    if (!ElectricWeapon)
    {
        ElectricWeapon = FindComponentByClass<UElectricWeapon>();
        if (ElectricWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character::BeginPlay - ElectricWeapon component was NULL, but successfully found and re-linked!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Character::BeginPlay - FATAL ERROR: Could not find ElectricWeapon component at all!"));
        }
    }

    if (!WaterWeapon)
    {
        WaterWeapon = FindComponentByClass<UWaterWeapon>();
        if (WaterWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character::BeginPlay - WaterWeapon component was NULL, but successfully found and re-linked!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Character::BeginPlay - FATAL ERROR: Could not find WaterWeapon component at all!"));
        }
    }

    if (!FlameWeapon)
    {
        FlameWeapon = FindComponentByClass<UFlameWeapon>();
        if (FlameWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character::BeginPlay - FlameWeapon component was NULL, but successfully found and re-linked!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Character::BeginPlay - FATAL ERROR: Could not find FlameWeapon component at all!"));
        }
    }
    // --- 물리 그랩 잠금 해제 아이템과의 오버랩 이벤트 바인딩 추가 ---
    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetGenerateOverlapEvents(true);
        CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &APrototypeCharacter::HandlePysGrabUnlockItemOverlap);
    }
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

void APrototypeCharacter::HandlePysGrabUnlockItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // "PysGrabUnlockTrigger" 태그를 가진 액터와 닿았는지 확인
    if (OtherActor->ActorHasTag(FName("PysGrabUnlockTrigger")))
    {
        UE_LOG(LogTemp, Log, TEXT("캐릭터가 물리 그랩 잠금 해제 아이템 ('%s')과 접촉!"), *OtherActor->GetName());

        // 1. GameInstance 가져와서 "영구적(세션 동안)"으로 기능 잠금 해제
        UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(GetGameInstance());
        if (GameInst)
        {
            GameInst->UnlockPysGrabFeature(); // GameInstance의 상태를 true로 변경
        }

        // 2. 현재 GameMode 가져와서 "즉시 현재 레벨에서" 기능 활성화 (false를 true로 변경)
        APrototypeGameMode* ProtoGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (ProtoGameMode)
        {
            ProtoGameMode->SetPysGrabActiveForLevel(true); // GameMode의 상태를 true로 변경
        }

        // 3. 이 아이템은 한 번만 작동하도록 파괴 (또는 비활성화)
        OtherActor->Destroy();
        UE_LOG(LogTemp, Log, TEXT("잠금 해제 아이템 '%s' 파괴됨."), *OtherActor->GetName());
    }
}

/* ---------- Tick ---------- */
void APrototypeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FString MapName = GetWorld()->GetMapName();
    MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    if (MapName == TEXT("GameStartMap"))
    {
        /* 오프닝용 맵에서 카메라 위치/회전 업데이트 */
        FVector CameraOffset = FVector(-210.f, -80.f, 0.f);
        FRotator CameraRotation = FRotator(0.f, 140.f, 0.f);
        FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
        CameraPivot->SetWorldLocation(NewLocation);
        CameraPivot->SetWorldRotation(CameraRotation);
    }
    else {
        /* 카메라 위치/회전 업데이트 */
        FVector CameraOffset = FVector(-1000.f, 0.f, 0.f); // 카메라 높이 0 -> 1000으로 수정
        FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f);
        FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
        CameraPivot->SetWorldLocation(NewLocation);
        CameraPivot->SetWorldRotation(CameraRotation);
    }
    /* 회전 로직 (기존 코드 유지) */
    if (IsRotationLocked()) {
        UE_LOG(LogTemp, Warning, TEXT("grabbbb"));

        return;
    }
    if (bShouldRotateToMouse)
    {
        UE_LOG(LogTemp, Warning, TEXT("1111"));

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
    PlayerInput->BindAction("Pause", IE_Pressed, this, &APrototypeCharacter::Pause);
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

	UpdateHPUI(); // UI 업데이트 호출
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
    IsDied();
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

FString APrototypeCharacter::GetCurrentWeaponName() const
{
    if (InputManager) // InputManager가 유효한지 확인
    {
        // InputManager의 새로운 public getter 함수들을 사용
        if (InputManager->IsElectricWeaponActive()) return TEXT("ElectricGun");
        if (InputManager->IsWaterWeaponActive())   return TEXT("WaterGun");
        if (InputManager->IsFlameWeaponActive())   return TEXT("FlameGun");
    }
    return TEXT("Shooter"); // 기본값 또는 InputManager가 없을 경우
}
bool APrototypeCharacter::IsRotationLocked() const
{
    // Shooter 변수가 혹시 nullptr인지 먼저 확인합니다.
    if (!Shooter)
    {
        UE_LOG(LogTemp, Error, TEXT("[CHAR IsRotationLocked] ERROR! My Shooter component variable is NULL!"));
        return false;
    }

    // Shooter가 유효하다면, 그 결과를 로그로 남깁니다.
    const bool bResult = Shooter->ShouldLockRotation();
    UE_LOG(LogTemp, Warning, TEXT("[CHAR IsRotationLocked] My Shooter at %p says ShouldLockRotation is: %s"), Shooter, bResult ? TEXT("TRUE") : TEXT("FALSE"));

    return bResult;
}
// 무기 변경 시 호출될 내부 함수 (새로 추가)
void APrototypeCharacter::NotifyWeaponChanged()
{
    CurrentWeaponDisplayName = GetCurrentWeaponName(); // 현재 무기 이름 업데이트
    OnWeaponChanged.Broadcast(CurrentWeaponDisplayName); // 델리게이트 호출

	UpdateWeaponUI(); // UI 업데이트 호출
    UE_LOG(LogTemp, Warning, TEXT("APrototypeCharacter::NotifyWeaponChanged - Broadcasting OnWeaponChanged. NewWeapon: %s"), *CurrentWeaponDisplayName);
}

/* --- 에너지 획득 함수 구현 (델리게이트 방송 추가) --- */
void APrototypeCharacter::SetGetBasicEnergy(bool bValue)
{
    basicEnergy--;
    UpdateBEnergyUI();
    OnBasicEnergyChanged.Broadcast(basicEnergy); // 델리게이트 방송
    UE_LOG(LogTemp, Log, TEXT("Basic Energy changed: %d"), basicEnergy);
}

void APrototypeCharacter::SetGetFlameEnergy(bool bValue)
{
    flameEnergy--;
    UpdateFEnergyUI();
    OnFlameEnergyChanged.Broadcast(flameEnergy); // 델리게이트 방송
    UE_LOG(LogTemp, Log, TEXT("Flame Energy changed: %d"), flameEnergy);
}

void APrototypeCharacter::SetGetWaterEnergy(bool bValue)
{
    waterEnergy--;
    UpdateWEnergyUI();
    OnWaterEnergyChanged.Broadcast(waterEnergy); // 델리게이트 방송
    UE_LOG(LogTemp, Log, TEXT("Water Energy changed: %d"), waterEnergy);
}

void APrototypeCharacter::SetGetElectricEnergy(bool bValue)
{
    electricEnergy--;
    UpdateEEnergyUI();
    OnElectricEnergyChanged.Broadcast(electricEnergy); // 델리게이트 방송
    UE_LOG(LogTemp, Log, TEXT("Electric Energy changed: %d"), electricEnergy);
}

void APrototypeCharacter::SetGetKey(bool bValue)
{
    Key--;
    UpdateKeyUI();
    OnKeyChanged.Broadcast(Key); // 델리게이트 방송
    UE_LOG(LogTemp, Log, TEXT("Key changed: %d"), electricEnergy);
}