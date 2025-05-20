#include "EnemyActor.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyActor::AEnemyActor()
{
    Tags.Add("Enemy"); //  태그

    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_GameTraceChannel2);  // Enemy
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);  // Bullet에만 반응

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Engine/BasicShapes/Cube"));
    if (SM.Succeeded())
        Mesh->SetStaticMesh(SM.Object);
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();

    // 스폰된 위치 기준으로 Patrol 좌표 계산
    const FVector Base = GetActorLocation();
    PatrolPointA = Base + PatrolOffsetA;
    PatrolPointB = Base + PatrolOffsetB;
}

void AEnemyActor::Freeze(float Seconds)
{
    if (bIsFrozen) return;  // 중복 방지

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

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFrozen)
        return; // 이동만 막고, 다른 Tick 로직은 여기에 넣으면 됨

    const float Speed = 150.f;
    const FVector Target = bGoingToB ? PatrolPointB : PatrolPointA;
    FVector Dir = (Target - GetActorLocation()).GetSafeNormal();
    SetActorLocation(GetActorLocation() + Dir * Speed * DeltaTime);

    if (FVector::DistSquared(GetActorLocation(), Target) < 25.f * 25.f)
        bGoingToB = !bGoingToB;
}


void AEnemyActor::ApplyElectroShockEffect()
{
    if (!ElectroShockFX) return;

    UNiagaraFunctionLibrary::SpawnSystemAttached(
        ElectroShockFX,
        Mesh,  // 루트 Mesh에 붙이기
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true  // AutoDestroy
    );
}

void AEnemyActor::Unfreeze()
{
    bIsFrozen = false;
}
