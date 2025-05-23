#include "Battery.h"
#include "Components/StaticMeshComponent.h"

/* ---------- ������ ---------- */
ABattery::ABattery()
{
    PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    SetRootComponent(BodyMesh);
    BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));

    /* 3ĭ¥�� ���� ǥ�� Mesh (Blueprint���� �޽ÿ��� ��ü ����) */
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

/* ---------- ���� ���� ---------- */
void ABattery::Charge()
{
    if (ChargeCount >= 3) return;

    ++ChargeCount;
    RemainingPowerTime = ChargeCount * ChargeUnit;     // 1ĭ=3��, 2ĭ=6��, 3ĭ=9��
    UpdateIndicators();
}

/* ---------- �׷��� ������Ʈ ---------- */
void ABattery::UpdateIndicators()
{
    for (int32 i = 0; i < Indicators.Num(); ++i)
        Indicators[i]->SetVisibility(i < ChargeCount);
}
