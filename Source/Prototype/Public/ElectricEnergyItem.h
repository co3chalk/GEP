// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnergyItem.h"
#include "ElectricEnergyItem.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AElectricEnergyItem : public AEnergyItem
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	float electricEnergyCount = 0;
};
