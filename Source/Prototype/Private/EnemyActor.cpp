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
}

// Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect �Լ����� ���� ���� ����
// ... (���� ���� �Լ���) ...
void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return;
    UE_LOG(LogTemp, Log, TEXT("%s Freeze called for %f seconds."), *GetNameSafe(this), Seconds);
    bIsFrozen = true;
    ApplyElectroShockEffect();
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &AEnemyActor::Unfreeze, Seconds, false);
    }
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

void AEnemyActor::Unfreeze()
{
    bIsFrozen = false;
    if (ElectroShockEffect)
    {
        ElectroShockEffect->Deactivate();
    }
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