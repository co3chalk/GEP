#include "EnemyActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyActor::AEnemyActor()
{
    Tags.Add("Enemy"); //  �±�

    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_GameTraceChannel2);  // Enemy
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);  // Bullet���� ����

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Engine/BasicShapes/Cube"));
    if (SM.Succeeded())
        Mesh->SetStaticMesh(SM.Object);
}

void AEnemyActor::BeginPlay()
{
    Super::BeginPlay();

    // ������ ��ġ �������� Patrol ��ǥ ���
    const FVector Base = GetActorLocation();
    PatrolPointA = Base + PatrolOffsetA;
    PatrolPointB = Base + PatrolOffsetB;
}

void AEnemyActor::Freeze(float Seconds)
{
    FrozenTimer = Seconds;
}

void AEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FrozenTimer > 0.f)
    {
        FrozenTimer -= DeltaTime;
        return;
    }

    const float Speed = 150.f;
    const FVector Target = bGoingToB ? PatrolPointB : PatrolPointA;
    FVector Dir = (Target - GetActorLocation()).GetSafeNormal();
    SetActorLocation(GetActorLocation() + Dir * Speed * DeltaTime);

    if (FVector::DistSquared(GetActorLocation(), Target) < 25.f * 25.f)
        bGoingToB = !bGoingToB;
}
