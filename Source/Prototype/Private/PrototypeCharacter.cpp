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
#include "Components/CapsuleComponent.h"     // GetCapsuleComponent() ���
#include "Kismet/GameplayStatics.h"      // GetGameInstance, GetGameMode �� ���
#include "PrototypeGameInstance.h"     // UPrototypeGameInstance Ŭ���� ���
#include "PrototypeGameMode.h"             // APrototypeGameMode Ŭ���� ���



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
    GetCharacterMovement()->JumpZVelocity = 500.f;      // 600�� �ΰ� ��������
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
    FlameWeapon = CreateDefaultSubobject<UFlameWeapon>(TEXT("FlameWeapon"));
    InputManager = CreateDefaultSubobject<UInputManager>(TEXT("InputManager"));

    /* ȭ����� �޽�/�ݶ��̴� (���� �ڵ� ����) */
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
    // --- ���� �׷� ��� ���� �����۰��� ������ �̺�Ʈ ���ε� �߰� ---
    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetGenerateOverlapEvents(true);
        CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &APrototypeCharacter::HandlePysGrabUnlockItemOverlap);
    }
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

void APrototypeCharacter::HandlePysGrabUnlockItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // "PysGrabUnlockTrigger" �±׸� ���� ���Ϳ� ��Ҵ��� Ȯ��
    if (OtherActor->ActorHasTag(FName("PysGrabUnlockTrigger")))
    {
        UE_LOG(LogTemp, Log, TEXT("ĳ���Ͱ� ���� �׷� ��� ���� ������ ('%s')�� ����!"), *OtherActor->GetName());

        // 1. GameInstance �����ͼ� "������(���� ����)"���� ��� ��� ����
        UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(GetGameInstance());
        if (GameInst)
        {
            GameInst->UnlockPysGrabFeature(); // GameInstance�� ���¸� true�� ����
        }

        // 2. ���� GameMode �����ͼ� "��� ���� ��������" ��� Ȱ��ȭ (false�� true�� ����)
        APrototypeGameMode* ProtoGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (ProtoGameMode)
        {
            ProtoGameMode->SetPysGrabActiveForLevel(true); // GameMode�� ���¸� true�� ����
        }

        // 3. �� �������� �� ���� �۵��ϵ��� �ı� (�Ǵ� ��Ȱ��ȭ)
        OtherActor->Destroy();
        UE_LOG(LogTemp, Log, TEXT("��� ���� ������ '%s' �ı���."), *OtherActor->GetName());
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
        /* �����׿� �ʿ��� ī�޶� ��ġ/ȸ�� ������Ʈ */
        FVector CameraOffset = FVector(-210.f, -80.f, 0.f);
        FRotator CameraRotation = FRotator(0.f, 140.f, 0.f);
        FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
        CameraPivot->SetWorldLocation(NewLocation);
        CameraPivot->SetWorldRotation(CameraRotation);
    }
    else {
        /* ī�޶� ��ġ/ȸ�� ������Ʈ */
        FVector CameraOffset = FVector(-1000.f, 0.f, 0.f); // ī�޶� ���� 0 -> 1000���� ����
        FRotator CameraRotation = FRotator(-45.f, 0.f, 0.f);
        FVector NewLocation = GetActorLocation() + CameraRotation.RotateVector(CameraOffset);
        CameraPivot->SetWorldLocation(NewLocation);
        CameraPivot->SetWorldRotation(CameraRotation);
    }
    /* ȸ�� ���� (���� �ڵ� ����) */
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
    PlayerInput->BindAction("Pause", IE_Pressed, this, &APrototypeCharacter::Pause);
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

	UpdateHPUI(); // UI ������Ʈ ȣ��
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

FString APrototypeCharacter::GetCurrentWeaponName() const
{
    if (InputManager) // InputManager�� ��ȿ���� Ȯ��
    {
        // InputManager�� ���ο� public getter �Լ����� ���
        if (InputManager->IsElectricWeaponActive()) return TEXT("ElectricGun");
        if (InputManager->IsWaterWeaponActive())   return TEXT("WaterGun");
        if (InputManager->IsFlameWeaponActive())   return TEXT("FlameGun");
    }
    return TEXT("Shooter"); // �⺻�� �Ǵ� InputManager�� ���� ���
}
bool APrototypeCharacter::IsRotationLocked() const
{
    // Shooter ������ Ȥ�� nullptr���� ���� Ȯ���մϴ�.
    if (!Shooter)
    {
        UE_LOG(LogTemp, Error, TEXT("[CHAR IsRotationLocked] ERROR! My Shooter component variable is NULL!"));
        return false;
    }

    // Shooter�� ��ȿ�ϴٸ�, �� ����� �α׷� ����ϴ�.
    const bool bResult = Shooter->ShouldLockRotation();
    UE_LOG(LogTemp, Warning, TEXT("[CHAR IsRotationLocked] My Shooter at %p says ShouldLockRotation is: %s"), Shooter, bResult ? TEXT("TRUE") : TEXT("FALSE"));

    return bResult;
}
// ���� ���� �� ȣ��� ���� �Լ� (���� �߰�)
void APrototypeCharacter::NotifyWeaponChanged()
{
    CurrentWeaponDisplayName = GetCurrentWeaponName(); // ���� ���� �̸� ������Ʈ
    OnWeaponChanged.Broadcast(CurrentWeaponDisplayName); // ��������Ʈ ȣ��

	UpdateWeaponUI(); // UI ������Ʈ ȣ��
    UE_LOG(LogTemp, Warning, TEXT("APrototypeCharacter::NotifyWeaponChanged - Broadcasting OnWeaponChanged. NewWeapon: %s"), *CurrentWeaponDisplayName);
}

/* --- ������ ȹ�� �Լ� ���� (��������Ʈ ��� �߰�) --- */
void APrototypeCharacter::SetGetBasicEnergy(bool bValue)
{
    basicEnergy--;
    UpdateBEnergyUI();
    OnBasicEnergyChanged.Broadcast(basicEnergy); // ��������Ʈ ���
    UE_LOG(LogTemp, Log, TEXT("Basic Energy changed: %d"), basicEnergy);
}

void APrototypeCharacter::SetGetFlameEnergy(bool bValue)
{
    flameEnergy--;
    UpdateFEnergyUI();
    OnFlameEnergyChanged.Broadcast(flameEnergy); // ��������Ʈ ���
    UE_LOG(LogTemp, Log, TEXT("Flame Energy changed: %d"), flameEnergy);
}

void APrototypeCharacter::SetGetWaterEnergy(bool bValue)
{
    waterEnergy--;
    UpdateWEnergyUI();
    OnWaterEnergyChanged.Broadcast(waterEnergy); // ��������Ʈ ���
    UE_LOG(LogTemp, Log, TEXT("Water Energy changed: %d"), waterEnergy);
}

void APrototypeCharacter::SetGetElectricEnergy(bool bValue)
{
    electricEnergy--;
    UpdateEEnergyUI();
    OnElectricEnergyChanged.Broadcast(electricEnergy); // ��������Ʈ ���
    UE_LOG(LogTemp, Log, TEXT("Electric Energy changed: %d"), electricEnergy);
}

void APrototypeCharacter::SetGetKey(bool bValue)
{
    Key--;
    UpdateKeyUI();
    OnKeyChanged.Broadcast(Key); // ��������Ʈ ���
    UE_LOG(LogTemp, Log, TEXT("Key changed: %d"), electricEnergy);
}