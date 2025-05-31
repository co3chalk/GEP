#include "EnemyActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyActor::AEnemyActor()
{
    Tags.Add("Enemy");
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_GameTraceChannel2); // Enemy
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // Bullet
    Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap); // WaterBullet

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Engine/BasicShapes/Cube"));
    if (SM.Succeeded())
        Mesh->SetStaticMesh(SM.Object);

    // ���̾ư��� ������Ʈ ���� �� ����
    ElectroShockEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectroShockEffect"));
    ElectroShockEffect->SetupAttachment(Mesh);
    ElectroShockEffect->SetAutoActivate(false); // ���� �� ���� ����
    ElectroShockEffect->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // �Ӹ� ��
    ElectroShockEffect->SetRelativeScale3D(FVector(1.5f)); // ũ�� Ű��

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX(TEXT("/Game/Effects/Basic_VFX/Niagara/NS_Basic_9"));
    if (FX.Succeeded())
        ElectroShockEffect->SetAsset(FX.Object);
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();
    const FVector Base = GetActorLocation();
    PatrolPointA = Base + PatrolOffsetA;
    PatrolPointB = Base + PatrolOffsetB;
}

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFrozen) return;

    //const float Speed = 150.f;
    const FVector Target = bGoingToB ? PatrolPointB : PatrolPointA;
    FVector Dir = (Target - GetActorLocation()).GetSafeNormal();
    SetActorLocation(GetActorLocation() + Dir * CurrentSpeed * DeltaTime);  //Speed->CurrentSpeed

    if (FVector::DistSquared(GetActorLocation(), Target) < 25.f * 25.f)
        bGoingToB = !bGoingToB;
}

void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return;

    bIsFrozen = true;
    ApplyElectroShockEffect();

    GetWorld()->GetTimerManager().SetTimer(
        FreezeTimerHandle,
        this,
        &AEnemyActor::Unfreeze,
        Seconds,
        false
    );
}

void AEnemyActor::ApplyElectroShockEffect()
{
    if (ElectroShockEffect)
    {
        // ������ �ʱ�ȭ
        ElectroShockEffect->Deactivate();
        ElectroShockEffect->SetActive(false); // ���� ��Ȱ��ȭ
        ElectroShockEffect->SetActive(true);  // �ٽ� �ѱ�
        ElectroShockEffect->Activate(true);   // �����
    }
}


void AEnemyActor::Unfreeze()
{
    bIsFrozen = false;

    if (ElectroShockEffect)
    {
        ElectroShockEffect->Deactivate();
        ElectroShockEffect->SetActive(false); // ����
    }
}

void AEnemyActor::Slowdown(float Seconds)
{
    CurrentSpeed = 100.f; // ������ �ӵ�
    GetWorld()->GetTimerManager().SetTimer(
        SlowdownTimerHandle,
        this,
        &AEnemyActor::RestoreSpeed,
        Seconds,
        false
    );
}

void AEnemyActor::RestoreSpeed()
{
    CurrentSpeed = Speed;
}
