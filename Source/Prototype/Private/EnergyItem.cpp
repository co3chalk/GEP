// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PrototypeCharacter.h"
// Sets default values
AEnergyItem::AEnergyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
    SetRootComponent(boxComp);
 
    boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    boxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    meshComp->SetupAttachment(boxComp);

    boxComp->OnComponentBeginOverlap.AddDynamic(this, &AEnergyItem::OnOverlapBegin);
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
        Player->SetGetEnergy(true); // ������ ȹ�� ó��

        //�� ���Ҹ��� ȹ�� ī��Ʈ �߰�, ������ ȹ�� �ǰ� �� ���ҿ� �°� 1�� �̻� �����ϸ� �߻� �����ϰ� ����
        switch (id) {
        case 0:
            Player->SetGetFlameEnergy(true);
            //���� �ҿ����� ���� ����
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
        
        // ������ ����
        Destroy();
    }
}