// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PrototypeCharacter.h"

// Sets default values
AEnergyItem::AEnergyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
    SetRootComponent(RootComponent);
 
    sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    sphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    meshComp->SetupAttachment(sphereComp);

    sphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnergyItem::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnergyItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnergyItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (APrototypeCharacter* Player = Cast<APrototypeCharacter>(OtherActor))
    {
        Player->SetGetEnergy(true); // 에너지 획득 처리

        //각 원소마다 획득 카운트 추가, 에너지 획득 되고 각 원소에 맞게 1개 이상 존재하면 발사 가능하게 변경
        switch (id) {
        case 0:
            Player->SetGetFlameEnergy(true);
            //얻은 불에너지 갯수 증가
            break;
        case 1:
            Player->SetGetWaterEnergy(true);
            //
            break;
        case 2:
            Player->SetGetElectricEnergy(true);
            //
            break;
        }
        
        // 아이템 제거
        Destroy();
    }
}