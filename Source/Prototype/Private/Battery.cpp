#include "Battery.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // UStaticMesh�� �ε��ϱ� ���� �ʿ��մϴ�.
#include "Engine/EngineTypes.h"       // ECollisionEnabled, ECollisionChannel ���� ���� �ʿ��մϴ�.
#include "PhysicsEngine/BodyInstance.h" // SetMassOverrideInKg�� ���� �ʿ��� �� �ֽ��ϴ�.

ABattery::ABattery()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. BodyMesh�� RootComponent�� ����
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    SetRootComponent(BodyMesh);

    // 2. ������ ����ƽ �޽� �Ҵ� (����: ���� �⺻ �Ǹ��� �޽�)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded())
    {
        BodyMesh->SetStaticMesh(CylinderMeshAsset.Object);
    }
    else
    {
        // �޽ø� ã�� ������ ��� �α׸� ����ų� �ٸ� �⺻ �޽ø� �����մϴ�.
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Cylinder Mesh for Battery!"));
        // ���: BodyMesh->SetStaticMesh(��Ÿ �⺻ �޽�);
    }

    // 3. BodyMesh�� �浹 ����
   
    BodyMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    // Ư�� ä�ο� ���� ������ �� �����ϰ� �����Ϸ���:
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // ���� �� ���� �浹 ��� Ȱ��ȭ
    BodyMesh->SetCollisionObjectType(ECC_PhysicsBody); // ����: ���� �ٵ� Ÿ������ ����
    BodyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block); // Bullet ä�ο� ���

    BodyMesh->SetSimulatePhysics(true); // ���� �ùķ��̼� Ȱ��ȭ
    BodyMesh->SetMassOverrideInKg(NAME_None, 10.0f, true); // ������ ���� ����

    /* 3ĭ¥�� ���� ǥ�� Mesh (Blueprint���� �޽ÿ��� ��ü ����) */
    for (int32 i = 0; i < 3; ++i)
    {
        UStaticMeshComponent* Cell = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Indicator%d"), i));
        Cell->SetupAttachment(BodyMesh); // Indicators�� BodyMesh�� �پ��ֵ��� ����
        Cell->SetRelativeLocation(FVector(0.f, 0.f, 10.f + i * 12.f));
        Cell->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �ε������ʹ� �浹 ����
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
