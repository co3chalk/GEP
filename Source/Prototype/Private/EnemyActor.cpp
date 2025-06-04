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
#include "Animation/AnimSequenceBase.h" // .h에 전방선언 했다면 .cpp에 인클루드
#include "Animation/AnimInstance.h"   // 스켈레탈 메시의 AnimInstance를 통해 애니메이션을 제어할 수도 있습니다 (고급).
                                      // 여기서는 PlayAnimation을 직접 사용합니다.
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
    UpdateAnimation();

}
// EnemyActor.cpp

void AEnemyActor::UpdateAnimation()
{
    if (!CharacterMesh || (!MoveAnimation && !IdleAnimation)) // 메시가 없거나 설정된 애니메이션이 하나도 없으면 실행 안 함
    {
        return;
    }

    // --- 시작: 수정된 이동 상태 판단 로직 ---
    bool bIsActuallyTryingToMove = false;

    // 얼어있지 않고, 순찰 지점이 있으며, 현재 순찰 지점 인덱스가 유효한 경우에만 이동 시도 여부 판단
    if (!bIsFrozen && PatrolPoints.Num() > 0 && PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
    {
        const FVector CurrentActorLocation = GetActorLocation();
        const FVector TargetPatrolLocation = PatrolPoints[CurrentPatrolPointIndex];

        // 현재 속도가 0보다 크고, 목표 지점에 아직 도달하지 않았다면 움직이려는 것으로 간주
        // (목표 지점과의 거리가 매우 가깝지 않은지도 함께 고려)
        if (CurrentSpeed > KINDA_SMALL_NUMBER && !CurrentActorLocation.Equals(TargetPatrolLocation, 5.0f)) // 5.0f는 도달 판정 허용 오차
        {
            // 실제로 이동할 방향이 있는지도 확인 (매우 중요하지는 않지만, 혹시 모를 상황 대비)
            FVector CalculatedMoveDirection = (TargetPatrolLocation - CurrentActorLocation).GetSafeNormal();
            if (!CalculatedMoveDirection.IsNearlyZero(0.01f))
            {
                bIsActuallyTryingToMove = true;
            }
        }
    }
    // --- 끝: 수정된 이동 상태 판단 로직 ---

    bool bIsCurrentlyMoving = bIsActuallyTryingToMove; // 최종 이동 상태 결정

    // 상태가 변경되었거나, 현재 애니메이션이 설정되지 않은 경우에만 애니메이션 변경
    // (이하 애니메이션 재생 로직은 기존과 동일)
    if (bIsCurrentlyMoving != bWasMovingLastFrame || CurrentPlayingAnimation == nullptr)
    {
        if (bIsCurrentlyMoving)
        {
            if (MoveAnimation && CurrentPlayingAnimation != MoveAnimation)
            {
                CharacterMesh->PlayAnimation(MoveAnimation, true); // true는 루핑 여부
                CurrentPlayingAnimation = MoveAnimation;
                UE_LOG(LogTemp, Log, TEXT("%s is now playing MoveAnimation. Speed: %f"), *GetNameSafe(this), CurrentSpeed);
            }
        }
        else // 멈췄을 때 또는 움직이려고 하지 않을 때
        {
            if (IdleAnimation && CurrentPlayingAnimation != IdleAnimation)
            {
                CharacterMesh->PlayAnimation(IdleAnimation, true); // true는 루핑 여부
                CurrentPlayingAnimation = IdleAnimation;
                UE_LOG(LogTemp, Log, TEXT("%s is now playing IdleAnimation"), *GetNameSafe(this));
            }
            else if (!IdleAnimation && CurrentPlayingAnimation == MoveAnimation) // 대기 애니메이션이 없고, 이전에 움직임 애니메이션을 재생했다면 중지
            {
                CharacterMesh->Stop();
                CurrentPlayingAnimation = nullptr;
                UE_LOG(LogTemp, Log, TEXT("%s stopped animation because IdleAnimation is null"), *GetNameSafe(this));
            }
        }
    }
    bWasMovingLastFrame = bIsCurrentlyMoving; // 현재 이동 상태를 다음 프레임을 위해 저장
}

// Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect 함수들은 변경 없이 유지
// ... (이하 기존 함수들) ...
// EnemyActor.cpp
// EnemyActor.cpp
void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return; // 이미 얼어있다면 중복 실행 방지
    UE_LOG(LogTemp, Log, TEXT("%s Freeze called for %f seconds. CurrentSpeed was: %f"), *GetNameSafe(this), Seconds, CurrentSpeed);

    bIsFrozen = true;
    CurrentSpeed = 0.0f; // 이동 속도를 0으로 설정

    ApplyElectroShockEffect(); // 감전 이펙트 재생
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &AEnemyActor::Unfreeze, Seconds, false);
    }

    // --- 시작: Freeze 시 애니메이션 상태를 즉시 변경하는 코드 추가 ---
    if (CharacterMesh) // CharacterMesh가 유효한지 확인
    {
        if (IdleAnimation) // 설정된 Idle 애니메이션이 있다면
        {
            CharacterMesh->PlayAnimation(IdleAnimation, true); // Idle 애니메이션 재생 (루핑)
            CurrentPlayingAnimation = IdleAnimation;          // 현재 재생 애니메이션 상태 업데이트
            UE_LOG(LogTemp, Log, TEXT("%s: Freeze - Explicitly playing IdleAnimation."), *GetNameSafe(this));
        }
        else // Idle 애니메이션이 없다면, 현재 애니메이션이라도 멈춤
        {
            CharacterMesh->Stop();
            CurrentPlayingAnimation = nullptr; // 현재 재생 애니메이션 상태 업데이트
            UE_LOG(LogTemp, Log, TEXT("%s: Freeze - Stopping animation (No IdleAnimation set)."), *GetNameSafe(this));
        }
    }
    bWasMovingLastFrame = false; // 다음 Unfreeze 후 UpdateAnimation이 상태를 제대로 판단하도록 초기화
    // --- 끝: 애니메이션 상태 변경 코드 ---
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
    CurrentSpeed = Speed; // <--- 중요: 원래 속도(Speed 멤버 변수)로 복원!
    UE_LOG(LogTemp, Log, TEXT("%s Unfreeze. CurrentSpeed restored to: %f"), *GetNameSafe(this), CurrentSpeed);

    if (ElectroShockEffect)
    {
        ElectroShockEffect->Deactivate();
    }
    // GetWorld()->GetTimerManager().ClearTimer(FreezeTimerHandle); // SetTimer의 bLooping이 false면 자동 클리어됨
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