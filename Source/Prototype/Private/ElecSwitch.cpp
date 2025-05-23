#include "ElecSwitch.h"
#include "Components/BoxComponent.h"
#include "MovingGround.h"

AElecSwitch::AElecSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->InitBoxExtent(FVector(32));
    Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
    SetRootComponent(Trigger);
}

void AElecSwitch::BeginPlay()
{
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AElecSwitch::OnBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AElecSwitch::OnEnd);
}

void AElecSwitch::OnBegin(UPrimitiveComponent* /*OverlappedComp*/,
    AActor*              /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32                /*OtherBodyIndex*/,
    bool                 /*bFromSweep*/,
    const FHitResult&    /*SweepResult*/)
{
    if (++OverlapCount == 1) SetTargetsPowered(true);
}

void AElecSwitch::OnEnd(UPrimitiveComponent* /*OverlappedComp*/,
    AActor*              /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32                /*OtherBodyIndex*/)
{
    if (--OverlapCount == 0) SetTargetsPowered(false);
}

void AElecSwitch::SetTargetsPowered(bool bOn)
{
    for (AActor* Target : PoweredTargets)
        if (auto* MG = Cast<AMovingGround>(Target))
            MG->SetPowered(bOn);
}
