// ElecSwitch.cpp (수정본)
#include "ElecSwitch.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h" // <-- 스태틱 메시 헤더 포함
#include "MovingGround.h"

AElecSwitch::AElecSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 스태틱 메시 컴포넌트 생성 및 루트로 설정
    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SetRootComponent(SwitchMesh);
    // ** 중요: 블루프린트 또는 여기서 스위치에 사용할 메시를 설정해주세요! **
    // 예: SwitchMesh->SetStaticMesh(...)

    // 2. 트리거 컴포넌트 생성 및 메시에 부착
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->InitBoxExtent(FVector(50.f)); // <-- 메시 크기에 맞게 조절하세요.
    Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
    Trigger->SetupAttachment(SwitchMesh); // <-- 루트(메시)에 부착
    Trigger->SetGenerateOverlapEvents(true); // <-- 오버랩 이벤트 활성화
}

void AElecSwitch::BeginPlay()
{
    Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AElecSwitch::OnBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AElecSwitch::OnEnd);
}

void AElecSwitch::OnBegin(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32                /*OtherBodyIndex*/,
    bool                 /*bFromSweep*/,
    const FHitResult&    /*SweepResult*/)
{
    if (++OverlapCount == 1) {
        SetTargetsPowered(true);
        UGameplayStatics::PlaySoundAtLocation(this, TriggerSound, GetActorLocation());
    }
}

void AElecSwitch::OnEnd(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32                /*OtherBodyIndex*/)
{
    if (--OverlapCount == 0) SetTargetsPowered(false);
}

void AElecSwitch::SetTargetsPowered(bool bOn)
{
    for (AActor* TargetActor : PoweredTargets)
    {
        if (AMovingGround* MovingGround = Cast<AMovingGround>(TargetActor))
        {
            MovingGround->SetPowered(bOn);
        }
        else if (TargetActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("AElecSwitch: Target actor '%s' is not a MovingGround (or other supported type)."), *TargetActor->GetName());
        }
    }
}