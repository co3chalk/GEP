#include "ElecSwitch.h"
#include "Components/BoxComponent.h"
#include "MovingGround.h" // AMovingGround�� ���� �����ؾ� �մϴ�.

AElecSwitch::AElecSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->InitBoxExtent(FVector(32));
    Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
    SetRootComponent(Trigger);
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
    if (++OverlapCount == 1) SetTargetsPowered(true);
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
    for (AActor* TargetActor : PoweredTargets) // ������ AActor*
    {
        // TargetActor�� AMovingGround* �� �����ϰ� ĳ����
        if (AMovingGround* MovingGround = Cast<AMovingGround>(TargetActor))
        {
            MovingGround->SetPowered(bOn);
        }
        else
        {
            // Debugging�� ���� ĳ���ÿ� ������ ��� �α׸� ���� �� �ֽ��ϴ�.
            // UE_LOG(LogTemp, Warning, TEXT("AElecSwitch: Non-MovingGround actor found in PoweredTargets!"));
        }
    }
}