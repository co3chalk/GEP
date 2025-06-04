#include "EnemyActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h" // 필요에 따라

AEnemyActor::AEnemyActor()
{
    // ... (기존 생성자 내용 동일)
    Tags.Add("Enemy");
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
    RootComponent = CapsuleComponent;
    CapsuleComponent->SetCapsuleHalfHeight(88.0f);
    CapsuleComponent->SetCapsuleRadius(34.0f);
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
    CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap);

    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(CapsuleComponent);
    CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
    CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ElectroShockEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectroShockEffect"));
    ElectroShockEffect->SetupAttachment(CharacterMesh);
    ElectroShockEffect->SetAutoActivate(false);
    // DefaultElectroShockFX UPROPERTY를 사용한다면 생성자에서 로드할 필요 없음
    // if (DefaultElectroShockFX) { ElectroShockEffect->SetAsset(DefaultElectroShockFX); }

    CurrentSpeed = Speed;
    CurrentPatrolPointIndex = 0; // 초기 인덱스
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();

    const FVector BaseLocation = GetActorLocation();
    PatrolPoints.Empty(); // 배열 비우기

    // 설정된 오프셋을 기반으로 실제 패트롤 지점 추가
    PatrolPoints.Add(BaseLocation + PatrolOffsetA);
    PatrolPoints.Add(BaseLocation + PatrolOffsetB);
    PatrolPoints.Add(BaseLocation + PatrolOffsetC);
    PatrolPoints.Add(BaseLocation + PatrolOffsetD);
    // 필요한 만큼 더 추가하거나, 특정 조건에 따라 추가할 수 있습니다.

    // 패트롤 지점이 유효한지 확인 (예: 최소 1개 이상이어야 순찰 가능)
    if (PatrolPoints.Num() > 0)
    {
        CurrentPatrolPointIndex = 0;
    }
    else
    {
        // 패트롤 지점이 없으면 Tick에서 패트롤 로직을 실행하지 않도록 처리 가능
        // 예를 들어, PrimaryActorTick.bCanEverTick = false; 또는 bCanPatrol = false; 같은 플래그 사용
        UE_LOG(LogTemp, Warning, TEXT("EnemyActor %s has no patrol points defined."), *GetName());
    }
}

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 패트롤 지점이 없거나, 얼어있으면 이동 및 회전 로직을 실행하지 않음
    if (PatrolPoints.Num() == 0 || bIsFrozen)
    {
        return;
    }

    const FVector CurrentLocation = GetActorLocation();
    // 현재 목표 패트롤 지점
    const FVector TargetLocation = PatrolPoints[CurrentPatrolPointIndex];

    FVector MoveDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

    if (!MoveDirection.IsNearlyZero())
    {
        FVector NewLocation = CurrentLocation + MoveDirection * CurrentSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        FRotator TargetRotation = MoveDirection.Rotation();
        // 부드러운 회전을 원하면 FMath::RInterpTo 사용
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
    }

    // 목표 지점 도달 체크 (거리 임계값은 조절 가능)
    const float DistanceToTargetSquared = FVector::DistSquared(CurrentLocation, TargetLocation);
    if (DistanceToTargetSquared < FMath::Square(50.f)) // 50유닛 이내로 접근하면 다음 지점으로
    {
        // 다음 패트롤 지점으로 인덱스 업데이트 (배열 끝에 도달하면 처음으로 순환)
        CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPoints.Num();
    }
}

// ... Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect 함수들은 기존과 거의 동일하게 유지 ...
// (ApplyElectroShockEffect 에서 이펙트 에셋 설정 부분이 DefaultElectroShockFX를 사용하도록 바뀔 수 있음)

void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return;
    bIsFrozen = true;
    ApplyElectroShockEffect();
    GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &AEnemyActor::Unfreeze, Seconds, false);
}

void AEnemyActor::ApplyElectroShockEffect()
{
    if (ElectroShockEffect)
    {
        if (DefaultElectroShockFX && ElectroShockEffect->GetAsset() != DefaultElectroShockFX)
        {
            ElectroShockEffect->SetAsset(DefaultElectroShockFX); // 에셋이 설정 안되어있으면 여기서 설정
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
    CurrentSpeed = Speed * 0.5f;
    GetWorld()->GetTimerManager().SetTimer(SlowdownTimerHandle, this, &AEnemyActor::RestoreSpeed, Seconds, false);
}

void AEnemyActor::RestoreSpeed()
{
    CurrentSpeed = Speed;
}