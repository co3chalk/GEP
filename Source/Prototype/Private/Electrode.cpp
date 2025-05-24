// Electrode.cpp (������)
#include "Electrode.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h" // <-- ����ƽ �޽� ��� ����
#include "UObject/ConstructorHelpers.h"   // <-- FObjectFinder ��� ���� ����
#include "Battery.h"
#include "MovingGround.h"

AElectrode::AElectrode()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. ����ƽ �޽� ������Ʈ ���� �� ��Ʈ�� ����
    ElectrodeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElectrodeMesh"));
    SetRootComponent(ElectrodeMesh);

    // �⺻ �Ǹ��� �޽� �ε� �õ� (���� ���·� ����ϱ� ����)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded())
    {
        ElectrodeMesh->SetStaticMesh(CylinderMeshAsset.Object);
        // ** �߿�: �������Ʈ�� �����Ϳ��� �� �޽��� �������� �����Ͽ� (Ư�� Z��) **
        // ** ���� ���� ���·� ����� �ּ���. ��: ������ (X:2.0, Y:2.0, Z:0.1) **
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Cylinder Mesh for Electrode!"));
    }
    // ���� �޽ð� �ٴ� ������ �ϵ��� �ݸ��� ���� (�ʿ信 ���� ����)
    ElectrodeMesh->SetCollisionProfileName(TEXT("BlockAll"));


    // 2. Ž�� ���� ���� �� �޽ÿ� ����
    Detector = CreateDefaultSubobject<UBoxComponent>(TEXT("Detector"));
    Detector->InitBoxExtent(FVector(50.f, 50.f, 60.f)); // <-- ���� ũ��� ���͸� ���̿� �°� ����
    Detector->SetCollisionProfileName(TEXT("OverlapAll"));
    Detector->SetupAttachment(ElectrodeMesh); // <-- ��Ʈ(�޽�)�� ����
    Detector->SetGenerateOverlapEvents(true); // <-- ������ �̺�Ʈ Ȱ��ȭ
    // ���͸��� ���� �� �ֵ��� Detector�� ��ġ�� �޽� �������� ��¦ �ø� �� �ֽ��ϴ�.
    Detector->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // ���� ��, ���� �ʿ�
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
    {
        if (ConnectedBattery.Get() == B)
        {
            ConnectedBattery = nullptr;
        }
    }
}


void AElectrode::SetTargetsPowered(bool bOn)
{
    for (AActor* TargetActor : PoweredTargets)
    {
        if (AMovingGround* MovingGround = Cast<AMovingGround>(TargetActor))
        {
            MovingGround->SetPowered(bOn);
        }
        else if (TargetActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("AElectrode: Target actor '%s' is not a MovingGround (or other supported type)."), *TargetActor->GetName());
        }
    }
}