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
        // 틱마다 남은 시간 감소
        RemainingPowerTime -= Dt;

        int32 ExpectedChargeCount = FMath::CeilToInt(RemainingPowerTime / ChargeUnit);

        // 현재 ChargeCount가 예상되는 ChargeCount보다 높다면 (즉, 한 칸이 줄어들어야 할 때)
        if (ChargeCount > ExpectedChargeCount)
        {
            ChargeCount = ExpectedChargeCount;
            // ChargeCount가 0보다 작아지는 것을 방지 (음수가 되는 경우)
            if (ChargeCount < 0)
            {
                ChargeCount = 0;
            }
            UpdateIndicators();
        }

        // 최종적으로 남은 시간이 0 이하라면 모든 충전 상태 해제
        if (RemainingPowerTime <= 0.f)
        {
            ChargeCount = 0;
            RemainingPowerTime = 0.f;
            UpdateIndicators(); // 최종 업데이트
        }
    }
}

/* ---------- 전원 충전 ---------- */
void ABattery::Charge()
{
    if (ChargeCount >= 3) return;

    ++ChargeCount;
    RemainingPowerTime = ChargeCount * ChargeUnit;
    UpdateIndicators();
}

/* ---------- 그래픽 업데이트 ---------- */
void ABattery::UpdateIndicators()
{
    for (int32 i = 0; i < Indicators.Num(); ++i)
        Indicators[i]->SetVisibility(i < ChargeCount);
}