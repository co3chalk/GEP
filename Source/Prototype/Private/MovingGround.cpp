#include "MovingGround.h"
#include "Components/StaticMeshComponent.h"

AMovingGround::AMovingGround()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AMovingGround::BeginPlay()
{
    Super::BeginPlay();

    ActivePos = GetActorLocation();
    HiddenPos = ActivePos + FVector(0, 0, MoveOffsetZ);

    if (bDisappearByMove)
        SetActorLocation(HiddenPos);
    else
        SetActorHiddenInGame(true);
}

void AMovingGround::Tick(float Dt)
{
    Super::Tick(Dt);

    if (bDisappearByMove)
    {
        const FVector Target = bPowered ? ActivePos : HiddenPos;
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), Target, Dt, MoveSpeed));
    }
}

void AMovingGround::SetPowered(bool bOn)
{
    bPowered = bOn;

    if (!bDisappearByMove)
    {
        SetActorHiddenInGame(!bOn);
        SetActorEnableCollision(bOn);
    }
}
