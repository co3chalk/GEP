// ElecSwitch.cpp (������)
#include "ElecSwitch.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h" // <-- ����ƽ �޽� ��� ����
#include "MovingGround.h"

AElecSwitch::AElecSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. ����ƽ �޽� ������Ʈ ���� �� ��Ʈ�� ����
    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SetRootComponent(SwitchMesh);
    // ** �߿�: �������Ʈ �Ǵ� ���⼭ ����ġ�� ����� �޽ø� �������ּ���! **
    // ��: SwitchMesh->SetStaticMesh(...)

    // 2. Ʈ���� ������Ʈ ���� �� �޽ÿ� ����
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->InitBoxExtent(FVector(50.f)); // <-- �޽� ũ�⿡ �°� �����ϼ���.
    Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
    Trigger->SetupAttachment(SwitchMesh); // <-- ��Ʈ(�޽�)�� ����
    Trigger->SetGenerateOverlapEvents(true); // <-- ������ �̺�Ʈ Ȱ��ȭ
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