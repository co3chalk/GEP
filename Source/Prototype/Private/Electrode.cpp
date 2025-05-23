#include "Electrode.h"
#include "Components/BoxComponent.h"
#include "Battery.h"
#include "MovingGround.h" // AMovingGround를 직접 포함해야 합니다.

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
    if (ABattery* B = Cast<ABattery>(Other))
        ConnectedBattery = nullptr;
}


void AElectrode::SetTargetsPowered(bool bOn)
{
    for (AActor* TargetActor : PoweredTargets) // 여전히 AActor*
    {
        // TargetActor를 AMovingGround* 로 안전하게 캐스팅
        if (AMovingGround* MovingGround = Cast<AMovingGround>(TargetActor))
        {
            MovingGround->SetPowered(bOn);
        }
        else
        {
            // Debugging을 위해 캐스팅에 실패한 경우 로그를 남길 수 있습니다.
            // UE_LOG(LogTemp, Warning, TEXT("AElectrode: Non-MovingGround actor found in PoweredTargets!"));
        }
    }
}