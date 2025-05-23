#include "Electrode.h"
#include "Components/BoxComponent.h"
#include "Battery.h"
#include "MovingGround.h"

AElectrode::AElectrode()
{
    PrimaryActorTick.bCanEverTick = true;

    Detector = CreateDefaultSubobject<UBoxComponent>(TEXT("Detector"));
    Detector->InitBoxExtent(FVector(40));
    Detector->SetCollisionProfileName(TEXT("OverlapAll"));
    SetRootComponent(Detector);
}

void AElectrode::BeginPlay()
{
    Super::BeginPlay();
    Detector->OnComponentBeginOverlap.AddDynamic(this, &AElectrode::OnBegin);
    Detector->OnComponentEndOverlap.AddDynamic(this, &AElectrode::OnEnd);
}

void AElectrode::Tick(float Dt)
{
    Super::Tick(Dt);

    bool bNowPowered = ConnectedBattery.IsValid() && ConnectedBattery->IsPowered();
    if (bNowPowered != bWasPoweredLastFrame)
    {
        SetTargetsPowered(bNowPowered);
        bWasPoweredLastFrame = bNowPowered;
    }
}

/* ---------- Overlap ---------- */
void AElectrode::OnBegin(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (ABattery* B = Cast<ABattery>(Other))
        ConnectedBattery = B;
}
void AElectrode::OnEnd(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32)
{
    if (Other == ConnectedBattery.Get())   ConnectedBattery = nullptr;
}

void AElectrode::SetTargetsPowered(bool bOn)
{
    for (AActor* T : PoweredTargets)
        if (auto* MG = Cast<AMovingGround>(T))
            MG->SetPowered(bOn);
}
