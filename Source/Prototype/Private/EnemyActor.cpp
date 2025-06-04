#include "EnemyActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h" // �ʿ信 ����

AEnemyActor::AEnemyActor()
{
    // ... (���� ������ ���� ����)
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
    // DefaultElectroShockFX UPROPERTY�� ����Ѵٸ� �����ڿ��� �ε��� �ʿ� ����
    // if (DefaultElectroShockFX) { ElectroShockEffect->SetAsset(DefaultElectroShockFX); }

    CurrentSpeed = Speed;
    CurrentPatrolPointIndex = 0; // �ʱ� �ε���
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();

    const FVector BaseLocation = GetActorLocation();
    PatrolPoints.Empty(); // �迭 ����

    // ������ �������� ������� ���� ��Ʈ�� ���� �߰�
    PatrolPoints.Add(BaseLocation + PatrolOffsetA);
    PatrolPoints.Add(BaseLocation + PatrolOffsetB);
    PatrolPoints.Add(BaseLocation + PatrolOffsetC);
    PatrolPoints.Add(BaseLocation + PatrolOffsetD);
    // �ʿ��� ��ŭ �� �߰��ϰų�, Ư�� ���ǿ� ���� �߰��� �� �ֽ��ϴ�.

    // ��Ʈ�� ������ ��ȿ���� Ȯ�� (��: �ּ� 1�� �̻��̾�� ���� ����)
    if (PatrolPoints.Num() > 0)
    {
        CurrentPatrolPointIndex = 0;
    }
    else
    {
        // ��Ʈ�� ������ ������ Tick���� ��Ʈ�� ������ �������� �ʵ��� ó�� ����
        // ���� ���, PrimaryActorTick.bCanEverTick = false; �Ǵ� bCanPatrol = false; ���� �÷��� ���
        UE_LOG(LogTemp, Warning, TEXT("EnemyActor %s has no patrol points defined."), *GetName());
    }
}

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ��Ʈ�� ������ ���ų�, ��������� �̵� �� ȸ�� ������ �������� ����
    if (PatrolPoints.Num() == 0 || bIsFrozen)
    {
        return;
    }

    const FVector CurrentLocation = GetActorLocation();
    // ���� ��ǥ ��Ʈ�� ����
    const FVector TargetLocation = PatrolPoints[CurrentPatrolPointIndex];

    FVector MoveDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

    if (!MoveDirection.IsNearlyZero())
    {
        FVector NewLocation = CurrentLocation + MoveDirection * CurrentSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        FRotator TargetRotation = MoveDirection.Rotation();
        // �ε巯�� ȸ���� ���ϸ� FMath::RInterpTo ���
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
    }

    // ��ǥ ���� ���� üũ (�Ÿ� �Ӱ谪�� ���� ����)
    const float DistanceToTargetSquared = FVector::DistSquared(CurrentLocation, TargetLocation);
    if (DistanceToTargetSquared < FMath::Square(50.f)) // 50���� �̳��� �����ϸ� ���� ��������
    {
        // ���� ��Ʈ�� �������� �ε��� ������Ʈ (�迭 ���� �����ϸ� ó������ ��ȯ)
        CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPoints.Num();
    }
}

// ... Freeze, Unfreeze, Slowdown, RestoreSpeed, ApplyElectroShockEffect �Լ����� ������ ���� �����ϰ� ���� ...
// (ApplyElectroShockEffect ���� ����Ʈ ���� ���� �κ��� DefaultElectroShockFX�� ����ϵ��� �ٲ� �� ����)

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
            ElectroShockEffect->SetAsset(DefaultElectroShockFX); // ������ ���� �ȵǾ������� ���⼭ ����
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