// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricEnergyItem.h"
#include "PrototypeCharacter.h"
#include "Components/BoxComponent.h"

void AElectricEnergyItem::BeginPlay()
{
	Super::BeginPlay();

	if (boxComp)
	{
		boxComp->OnComponentBeginOverlap.AddDynamic(this, &AElectricEnergyItem::OnOverlapBegin);
	}
}

void AElectricEnergyItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APrototypeCharacter* Character = Cast<APrototypeCharacter>(OtherActor))
	{
		Character->SetGetElectricEnergy(true); // Àü±â ¿¡³ÊÁö È¹µæ
		Destroy();
	}
}