// EnemyActor.cpp
#include "EnemyActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/BillboardComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimSequenceBase.h" // .h�� ���漱�� �ߴٸ� .cpp�� ��Ŭ���
#include "Animation/AnimInstance.h"   // ���̷�Ż �޽��� AnimInstance�� ���� �ִϸ��̼��� ������ ���� �ֽ��ϴ� (���).
                                      // ���⼭�� PlayAnimation�� ���� ����մϴ�.
#include "ElecBullet.h"
#include "WaterBullet.h"


AEnemyActor::AEnemyActor()
{
    Tags.Add("Enemy");
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
    RootComponent = CapsuleComponent;
    CapsuleComponent->SetCapsuleHalfHeight(88.0f);
    CapsuleComponent->SetCapsuleRadius(34.0f);

    // �ݸ��� Ȱ��ȭ �� ������Ʈ Ÿ�� ����
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel2); // "Enemy" ä��

    // �⺻������ ��� ä�� ����
    CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    // Ư�� ä�ο� ���� ���� ����
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // �÷��̾� Pawn���� ������
    CapsuleComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // �� ��� Block (�ʿ信 ����)

    // ElecBullet (ECC_GameTraceChannel1)
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    // WaterBullet (ECC_GameTraceChannel10)
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap);


    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(CapsuleComponent);
    CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
    CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // ���̷�Ż �޽ô� ���� ��ü �ݸ��� ��� �� ��

    ElectroShockEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectroShockEffect"));
    ElectroShockEffect->SetupAttachment(CharacterMesh);
    ElectroShockEffect->SetAutoActivate(false);

#if WITH_EDITORONLY_DATA
    auto CreateVisualizer = [this](FName ComponentName, const FVector& Offset) -> UBillboardComponent* {
        UBillboardComponent* Billboard = CreateDefaultSubobject<UBillboardComponent>(ComponentName);
        if (Billboard)
        {
            Billboard->SetupAttachment(RootComponent);
            Billboard->SetRelativeLocation(Offset);
            Billboard->bIsEditorOnly = true;
            Billboard->bHiddenInGame = true;
        }
        return Billboard;
    };
    PatrolPointVisualizerA = CreateVisualizer(TEXT("PatrolVisualizerA"), PatrolOffsetA);
    PatrolPointVisualizerB = CreateVisualizer(TEXT("PatrolVisualizerB"), PatrolOffsetB);
    PatrolPointVisualizerC = CreateVisualizer(TEXT("PatrolVisualizerC"), PatrolOffsetC);
    PatrolPointVisualizerD = CreateVisualizer(TEXT("PatrolVisualizerD"), PatrolOffsetD);
#endif

    CurrentSpeed = Speed;
    CurrentPatrolPointIndex = 0;
    MoveAnimation = nullptr;
    IdleAnimation = nullptr;
    CurrentPlayingAnimation = nullptr;
    bWasMovingLastFrame = false;
}

void AEnemyActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
    if (PatrolPointVisualizerA) PatrolPointVisualizerA->SetRelativeLocation(PatrolOffsetA);
    if (PatrolPointVisualizerB) PatrolPointVisualizerB->SetRelativeLocation(PatrolOffsetB);
    if (PatrolPointVisualizerC) PatrolPointVisualizerC->SetRelativeLocation(PatrolOffsetC);
    if (PatrolPointVisualizerD) PatrolPointVisualizerD->SetRelativeLocation(PatrolOffsetD);
#endif
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();

    if (CapsuleComponent)
    {
        CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyActor::OnCapsuleBeginOverlap);
    }

    const FVector BaseLocation = GetActorLocation();
    PatrolPoints.Empty();
    PatrolPoints.Add(BaseLocation + PatrolOffsetA);
    PatrolPoints.Add(BaseLocation + PatrolOffsetB);
    PatrolPoints.Add(BaseLocation + PatrolOffsetC);
    PatrolPoints.Add(BaseLocation + PatrolOffsetD);

    if (PatrolPoints.Num() > 0)
    {
        CurrentPatrolPointIndex = 0;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyActor %s has no patrol points defined."), *GetNameSafe(this));
    }
}

void AEnemyActor::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && !OtherActor->IsPendingKill())
    {
        if (AElecBullet* ElecBullet = Cast<AElecBullet>(OtherActor))
        {
            UE_LOG(LogTemp, Log, TEXT("%s overlapped with ElecBullet: %s. Applying Freeze."), *GetNameSafe(this), *OtherActor->GetName());
            Freeze(FreezeDurationOnHit);
            // ElecBullet�� ���⼭ �ı�����, ElecBullet�� ������ �ı������� ���� ������ ���� ����
            // ElecBullet->Destroy(); 
        }
        else if (AWaterBullet* WaterBullet = Cast<AWaterBullet>(OtherActor))
        {
            UE_LOG(LogTemp, Log, TEXT("%s overlapped with WaterBullet: %s. Applying Slowdown."), *GetNameSafe(this), *OtherActor->GetName());
            Slowdown(SlowdownDurationOnWaterHit);
            // WaterBullet�� ������ OnOverlapEnemy���� �ı� ������ ������ ���� �� ����
            // �Ǵ� ���⼭ WaterBullet->DestroyBullet(); ���� ȣ��
        }
    }
}

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PatrolPoints.Num() == 0 || bIsFrozen)
    {
        return;
    }

    const FVector CurrentLocation = GetActorLocation();
    const FVector TargetLocation = PatrolPoints[CurrentPatrolPointIndex];
    FVector MoveDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

    if (!MoveDirection.IsNearlyZero())
    {
        FVector NewLocation = CurrentLocation + MoveDirection * CurrentSpeed * DeltaTime;
        SetActorLocation(NewLocation);
        FRotator TargetRotation = MoveDirection.Rotation();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
    }

    const float DistanceToTargetSquared = FVector::DistSquared(CurrentLocation, TargetLocation);
    if (DistanceToTargetSquared < FMath::Square(50.f)) // 50���� ���� ��
    {
        CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPoints.Num();
    }
    UpdateAnimation();

}
// EnemyActor.cpp

void AEnemyActor::UpdateAnimation()
{
    if (!CharacterMesh || (!MoveAnimation && !IdleAnimation)) // �޽ð� ���ų� ������ �ִϸ��̼��� �ϳ��� ������ ���� �� ��
    {
        return;
    }

    // --- ����: ������ �̵� ���� �Ǵ� ���� ---
    bool bIsActuallyTryingToMove = false;

    // ������� �ʰ�, ���� ������ ������, ���� ���� ���� �ε����� ��ȿ�� ��쿡�� �̵� �õ� ���� �Ǵ�
    if (!bIsFrozen && PatrolPoints.Num() > 0 && PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
    {
        const FVector CurrentActorLocation = GetActorLocation();
        const FVector TargetPatrolLocation = PatrolPoints[CurrentPatrolPointIndex];

        // ���� �ӵ��� 0���� ũ��, ��ǥ ������ ���� �������� �ʾҴٸ� �����̷��� ������ ����
        // (��ǥ �������� �Ÿ��� �ſ� ������ �������� �Բ� ���)
        if (CurrentSpeed > KINDA_SMALL_NUMBER && !CurrentActorLocation.Equals(TargetPatrolLocation, 5.0f)) // 5.0f�� ���� ���� ��� ����
        {
            // ������ �̵��� ������ �ִ����� Ȯ�� (�ſ� �߿������� ������, Ȥ�� �� ��Ȳ ���)
            FVector CalculatedMoveDirection = (TargetPatrolLocation - CurrentActorLocation).GetSafeNormal();
            if (!CalculatedMoveDirection.IsNearlyZero(0.01f))
            {
                bIsActuallyTryingToMove = true;
            }
        }
    }
    // --- ��: ������ �̵� ���� �Ǵ� ���� ---

    bool bIsCurrentlyMoving = bIsActuallyTryingToMove; // ���� �̵� ���� ����

    // ���°� ����Ǿ��ų�, ���� �ִϸ��̼��� �������� ���� ��쿡�� �ִϸ��̼� ����
    // (���� �ִϸ��̼� ��� ������ ������ ����)
    if (bIsCurrentlyMoving != bWasMovingLastFrame || CurrentPlayingAnimation == nullptr)
    {
        if (bIsCurrentlyMoving)
        {
            if (MoveAnimation && CurrentPlayingAnimation != MoveAnimation)
            {
                CharacterMesh->PlayAnimation(MoveAnimation, true); // true�� ���� ����
                CurrentPlayingAnimation = MoveAnimation;
                UE_LOG(LogTemp, Log, TEXT("%s is now playing MoveAnimation. Speed: %f"), *GetNameSafe(this), CurrentSpeed);
            }
        }
        else // ������ �� �Ǵ� �����̷��� ���� ���� ��
        {
            if (IdleAnimation && CurrentPlayingAnimation != IdleAnimation)
            {
                CharacterMesh->PlayAnimation(IdleAnimation, true); // true�� ���� ����
                CurrentPlayingAnimation = IdleAnimation;
                UE_LOG(LogTemp, Log, TEXT("%s is now playing IdleAnimation"), *GetNameSafe(this));
            }
            else if (!IdleAnimation && CurrentPlayingAnimation == MoveAnimation) // ��� �ִϸ��̼��� ����, ������ ������ �ִϸ��̼��� ����ߴٸ� ����
            {
                CharacterMesh->Stop();
                CurrentPlayingAnimation = nullptr;
                UE_LOG(LogTemp, Log, TEXT("%s stopped animation because IdleAnimation is null"), *GetNameSafe(this));
            }
        }
    }
    bWasMovingLastFrame = bIsCurrentlyMoving; // ���� �̵� ���¸� ���� �������� ���� ����
}

// Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect �Լ����� ���� ���� ����
// ... (���� ���� �Լ���) ...
// EnemyActor.cpp
// EnemyActor.cpp
void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return; // �̹� ����ִٸ� �ߺ� ���� ����
    UE_LOG(LogTemp, Log, TEXT("%s Freeze called for %f seconds. CurrentSpeed was: %f"), *GetNameSafe(this), Seconds, CurrentSpeed);

    bIsFrozen = true;
    CurrentSpeed = 0.0f; // �̵� �ӵ��� 0���� ����

    ApplyElectroShockEffect(); // ���� ����Ʈ ���
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &AEnemyActor::Unfreeze, Seconds, false);
    }

    // --- ����: Freeze �� �ִϸ��̼� ���¸� ��� �����ϴ� �ڵ� �߰� ---
    if (CharacterMesh) // CharacterMesh�� ��ȿ���� Ȯ��
    {
        if (IdleAnimation) // ������ Idle �ִϸ��̼��� �ִٸ�
        {
            CharacterMesh->PlayAnimation(IdleAnimation, true); // Idle �ִϸ��̼� ��� (����)
            CurrentPlayingAnimation = IdleAnimation;          // ���� ��� �ִϸ��̼� ���� ������Ʈ
            UE_LOG(LogTemp, Log, TEXT("%s: Freeze - Explicitly playing IdleAnimation."), *GetNameSafe(this));
        }
        else // Idle �ִϸ��̼��� ���ٸ�, ���� �ִϸ��̼��̶� ����
        {
            CharacterMesh->Stop();
            CurrentPlayingAnimation = nullptr; // ���� ��� �ִϸ��̼� ���� ������Ʈ
            UE_LOG(LogTemp, Log, TEXT("%s: Freeze - Stopping animation (No IdleAnimation set)."), *GetNameSafe(this));
        }
    }
    bWasMovingLastFrame = false; // ���� Unfreeze �� UpdateAnimation�� ���¸� ����� �Ǵ��ϵ��� �ʱ�ȭ
    // --- ��: �ִϸ��̼� ���� ���� �ڵ� ---
}

void AEnemyActor::ApplyElectroShockEffect()
{
    if (ElectroShockEffect)
    {
        if (DefaultElectroShockFX && ElectroShockEffect->GetAsset() != DefaultElectroShockFX)
        {
            ElectroShockEffect->SetAsset(DefaultElectroShockFX);
        }
        ElectroShockEffect->Activate(true);
    }
}

// EnemyActor.cpp
void AEnemyActor::Unfreeze()
{
    bIsFrozen = false;
    CurrentSpeed = Speed; // <--- �߿�: ���� �ӵ�(Speed ��� ����)�� ����!
    UE_LOG(LogTemp, Log, TEXT("%s Unfreeze. CurrentSpeed restored to: %f"), *GetNameSafe(this), CurrentSpeed);

    if (ElectroShockEffect)
    {
        ElectroShockEffect->Deactivate();
    }
    // GetWorld()->GetTimerManager().ClearTimer(FreezeTimerHandle); // SetTimer�� bLooping�� false�� �ڵ� Ŭ�����
}

void AEnemyActor::Slowdown(float Seconds)
{
    CurrentSpeed = Speed * 0.5f; // ���� �ӵ����� ��������
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(SlowdownTimerHandle, this, &AEnemyActor::RestoreSpeed, Seconds, false);
    }
}

void AEnemyActor::RestoreSpeed()
{
    CurrentSpeed = Speed; // ���� �ӵ��� ����
}