// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
AWaterSwitch::AWaterSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SetRootComponent(SwitchMesh);

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->InitBoxExtent(FVector(50.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
    Trigger->SetupAttachment(SwitchMesh);
    Trigger->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AWaterSwitch::BeginPlay()
{
	Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AWaterSwitch::OnBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AWaterSwitch::OnEnd);
}

// Called every frame
void AWaterSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaterSwitch::OnBegin(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32 /*OtherBodyIndex*/,
    bool /*bFromSweep*/,
    const FHitResult& /*SweepResult*/)
{
    UE_LOG(LogTemp, Warning, TEXT("Water Trigger on!")); //
    if (++OverlapCount == 1)
    {
        SetTargetsPowered(true);
    }
}

void AWaterSwitch::OnEnd(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* /*OtherActor*/,
    UPrimitiveComponent* /*OtherComp*/,
    int32 /*OtherBodyIndex*/)
{
    if (--OverlapCount == 0)
    {
        SetTargetsPowered(false);
    }
}

void AWaterSwitch::SetTargetsPowered(bool bOn)
{
    for (AActor* TargetActor : PoweredTargets)
    {
        if (!TargetActor) continue;

        if (bOn)
        {
            FRotator NewRotation = TargetActor->GetActorRotation();
            NewRotation.Roll += 180.f;
            TargetActor->SetActorRotation(NewRotation);
        }
    }
}