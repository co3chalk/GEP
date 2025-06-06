// Battery.cpp

#include "Battery.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/EngineTypes.h"
#include "PhysicsEngine/BodyInstance.h"

ABattery::ABattery()
{
    PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    SetRootComponent(BodyMesh);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded())
    {
        BodyMesh->SetStaticMesh(CylinderMeshAsset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Cylinder Mesh for Battery!"));
    }

    BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));
    //BodyMesh->SetSimulatePhysics(true);
    //BodyMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);

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

    BodyMesh->SetSimulatePhysics(true);
    BodyMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);
}

void ABattery::Tick(float Dt)
{
    Super::Tick(Dt);

    if (ChargeCount > 0)
    {
        // ƽ���� ���� �ð� ����
        RemainingPowerTime -= Dt;

        int32 ExpectedChargeCount = FMath::CeilToInt(RemainingPowerTime / ChargeUnit);

        // ���� ChargeCount�� ����Ǵ� ChargeCount���� ���ٸ� (��, �� ĭ�� �پ���� �� ��)
        if (ChargeCount > ExpectedChargeCount)
        {
            ChargeCount = ExpectedChargeCount;
            // ChargeCount�� 0���� �۾����� ���� ���� (������ �Ǵ� ���)
            if (ChargeCount < 0)
            {
                ChargeCount = 0;
            }
            UpdateIndicators();
        }

        // ���������� ���� �ð��� 0 ���϶�� ��� ���� ���� ����
        if (RemainingPowerTime <= 0.f)
        {
            ChargeCount = 0;
            RemainingPowerTime = 0.f;
            UpdateIndicators(); // ���� ������Ʈ
        }
    }
}

/* ---------- ���� ���� ---------- */
void ABattery::Charge()
{
    if (ChargeCount >= 3) return;

    ++ChargeCount;
    RemainingPowerTime = ChargeCount * ChargeUnit;
    UpdateIndicators();
}

/* ---------- �׷��� ������Ʈ ---------- */
void ABattery::UpdateIndicators()
{
    for (int32 i = 0; i < Indicators.Num(); ++i)
        Indicators[i]->SetVisibility(i < ChargeCount);
}