#include "Battery.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // UStaticMesh를 로드하기 위해 필요합니다.
#include "Engine/EngineTypes.h"       // ECollisionEnabled, ECollisionChannel 등을 위해 필요합니다.
#include "PhysicsEngine/BodyInstance.h" // SetMassOverrideInKg를 위해 필요할 수 있습니다.

ABattery::ABattery()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. BodyMesh를 RootComponent로 설정
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    SetRootComponent(BodyMesh);

    // 2. 원통형 스태틱 메시 할당 (예시: 엔진 기본 실린더 메시)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded())
    {
        BodyMesh->SetStaticMesh(CylinderMeshAsset.Object);
    }
    else
    {
        // 메시를 찾지 못했을 경우 로그를 남기거나 다른 기본 메시를 설정합니다.
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Cylinder Mesh for Battery!"));
        // 대안: BodyMesh->SetStaticMesh(기타 기본 메시);
    }

    // 3. BodyMesh의 충돌 설정
   
    BodyMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    // 특정 채널에 대한 응답을 더 세밀하게 제어하려면:
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 질의 및 물리 충돌 모두 활성화
    BodyMesh->SetCollisionObjectType(ECC_PhysicsBody); // 예시: 물리 바디 타입으로 설정
    BodyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block); // Bullet 채널에 블록

    BodyMesh->SetSimulatePhysics(true); // 물리 시뮬레이션 활성화
    BodyMesh->SetMassOverrideInKg(NAME_None, 10.0f, true); // 적절한 질량 설정

    /* 3칸짜리 충전 표시 Mesh (Blueprint에서 메시에셋 교체 가능) */
    for (int32 i = 0; i < 3; ++i)
    {
        UStaticMeshComponent* Cell = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Indicator%d"), i));
        Cell->SetupAttachment(BodyMesh); // Indicators는 BodyMesh에 붙어있도록 유지
        Cell->SetRelativeLocation(FVector(0.f, 0.f, 10.f + i * 12.f));
        Cell->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 인디케이터는 충돌 없음
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
