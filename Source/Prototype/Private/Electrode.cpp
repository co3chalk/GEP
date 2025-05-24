// Electrode.cpp (수정본)
#include "Electrode.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h" // <-- 스태틱 메시 헤더 포함
#include "UObject/ConstructorHelpers.h"   // <-- FObjectFinder 사용 위해 포함
#include "Battery.h"
#include "MovingGround.h"

AElectrode::AElectrode()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 스태틱 메시 컴포넌트 생성 및 루트로 설정
    ElectrodeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElectrodeMesh"));
    SetRootComponent(ElectrodeMesh);

    // 기본 실린더 메시 로드 시도 (원반 형태로 사용하기 위함)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded())
    {
        ElectrodeMesh->SetStaticMesh(CylinderMeshAsset.Object);
        // ** 중요: 블루프린트나 에디터에서 이 메시의 스케일을 조절하여 (특히 Z축) **
        // ** 얇은 원반 형태로 만들어 주세요. 예: 스케일 (X:2.0, Y:2.0, Z:0.1) **
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Cylinder Mesh for Electrode!"));
    }
    // 전극 메시가 바닥 역할을 하도록 콜리전 설정 (필요에 따라 변경)
    ElectrodeMesh->SetCollisionProfileName(TEXT("BlockAll"));


    // 2. 탐지 볼륨 생성 및 메시에 부착
    Detector = CreateDefaultSubobject<UBoxComponent>(TEXT("Detector"));
    Detector->InitBoxExtent(FVector(50.f, 50.f, 60.f)); // <-- 원반 크기와 배터리 높이에 맞게 조절
    Detector->SetCollisionProfileName(TEXT("OverlapAll"));
    Detector->SetupAttachment(ElectrodeMesh); // <-- 루트(메시)에 부착
    Detector->SetGenerateOverlapEvents(true); // <-- 오버랩 이벤트 활성화
    // 배터리가 놓일 수 있도록 Detector의 위치를 메시 위쪽으로 살짝 올릴 수 있습니다.
    Detector->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // 예시 값, 조절 필요
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