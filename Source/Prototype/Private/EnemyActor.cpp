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

    // 콜리전 활성화 및 오브젝트 타입 설정
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel2); // "Enemy" 채널

    // 기본적으로 모든 채널 무시
    CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    // 특정 채널에 대한 반응 설정
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어 Pawn과의 오버랩
    CapsuleComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 벽 등과 Block (필요에 따라)

    // ElecBullet (ECC_GameTraceChannel1)
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    // WaterBullet (ECC_GameTraceChannel10)
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap);


    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(CapsuleComponent);
    CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
    CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 스켈레탈 메시는 보통 자체 콜리전 사용 안 함

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
            // ElecBullet을 여기서 파괴할지, ElecBullet이 스스로 파괴될지는 게임 로직에 따라 결정
            // ElecBullet->Destroy(); 
        }
        else if (AWaterBullet* WaterBullet = Cast<AWaterBullet>(OtherActor))
        {
            UE_LOG(LogTemp, Log, TEXT("%s overlapped with WaterBullet: %s. Applying Slowdown."), *GetNameSafe(this), *OtherActor->GetName());
            Slowdown(SlowdownDurationOnWaterHit);
            // WaterBullet은 스스로 OnOverlapEnemy에서 파괴 로직을 가지고 있을 수 있음
            // 또는 여기서 WaterBullet->DestroyBullet(); 등을 호출
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
    if (DistanceToTargetSquared < FMath::Square(50.f)) // 50유닛 근접 시
    {
        CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPoints.Num();
    }
}

// Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect 함수들은 변경 없이 유지
// ... (이하 기존 함수들) ...
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
    CurrentSpeed = Speed * 0.5f; // 기존 속도에서 절반으로
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(SlowdownTimerHandle, this, &AEnemyActor::RestoreSpeed, Seconds, false);
    }
}

void AEnemyActor::RestoreSpeed()
{
    CurrentSpeed = Speed; // 원래 속도로 복구
}