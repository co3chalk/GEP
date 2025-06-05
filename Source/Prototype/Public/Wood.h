// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wood.generated.h"

UCLASS()
class PROTOTYPE_API AWood : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWood();

	UPROPERTY(EditAnywhere)
	class USoundBase* BurnSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnFlameOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,	const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	float BurnDelay = 1.5f;

	FTimerHandle BurnTimer;

	void BurnUp();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
