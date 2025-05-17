// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterFlow.h"
#include "Components/BoxComponent.h"
#include "Prototype/PrototypeCharacter.h" // 플레이어 클래스
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
AWaterFlow::AWaterFlow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    RootComponent = BoxComp;

    BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    BoxComp->SetBoxExtent(FVector(200.f, 200.f, 100.f));
}

// Called when the game starts or when spawned
void AWaterFlow::BeginPlay()
{
	Super::BeginPlay();
    BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AWaterFlow::OnOverlapBegin);
    BoxComp->OnComponentEndOverlap.AddDynamic(this, &AWaterFlow::OnOverlapEnd);
}

// Called every frame
void AWaterFlow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    for (APrototypeCharacter* Character : OverlappingCharacters)
    {
        if (Character)
        {
            FVector ActualFlowDir = GetActorRotation().RotateVector(FlowDirection);
            Character->AddMovementInput(ActualFlowDir.GetSafeNormal(), FlowStrength * DeltaTime);

        }
    }
}

void AWaterFlow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (APrototypeCharacter* Character = Cast<APrototypeCharacter>(OtherActor))
    {
        OverlappingCharacters.Add(Character);
    }
}

void AWaterFlow::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (APrototypeCharacter* Character = Cast<APrototypeCharacter>(OtherActor))
    {
        OverlappingCharacters.Remove(Character);
    }
}

