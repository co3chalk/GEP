#include "Battery.h"
#include "Components/StaticMeshComponent.h"

/* ---------- 생성자 ---------- */
ABattery::ABattery()
{
    PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    SetRootComponent(BodyMesh);
    BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));

    /* 3칸짜리 충전 표시 Mesh (Blueprint에서 메시에셋 교체 가능) */
    for (int32 i = 0; i < 3; ++i)
    {
        UStaticMeshComponent* Cell = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Indicator%d"), i));
        Cell->SetupAttachment(BodyMesh);
        Cell->SetRelativeLocation(FVector(0.f, 0.f, 10.f + i * 12.f));
        Cell->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Indicators.Add(Cell);
    }
}

/* ---------- BeginPlay / Tick ---------- */
void ABattery::BeginPlay()
{
    Super::BeginPlay();
    UpdateIndicators();
}

void ABattery::Tick(float Dt)
{
    Super::Tick(Dt);

    if (ChargeCount > 0)
    {
        RemainingPowerTime -= Dt;
        if (RemainingPowerTime <= 0.f)
        {
            ChargeCount = 0;
            RemainingPowerTime = 0.f;
            UpdateIndicators();
        }
    }
}

/* ---------- 전원 충전 ---------- */
void ABattery::Charge()
{
    if (ChargeCount >= 3) return;

    ++ChargeCount;
    RemainingPowerTime = ChargeCount * ChargeUnit;     // 1칸=3초, 2칸=6초, 3칸=9초
    UpdateIndicators();
}

/* ---------- 그래픽 업데이트 ---------- */
void ABattery::UpdateIndicators()
{
    for (int32 i = 0; i < Indicators.Num(); ++i)
        Indicators[i]->SetVisibility(i < ChargeCount);
}
