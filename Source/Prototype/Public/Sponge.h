// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sponge.generated.h"

class UStaticMeshComponent;
class UPhysicsConstraintComponent;

UCLASS()
class PROTOTYPE_API ASponge : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASponge();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* staticMesh;

	UPROPERTY(EditAnywhere, Category = "Weight")
	float CurrentWeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Weight")
	float WeightThreshold = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* ChangedMaterial;

	UFUNCTION()
	void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void LockMotion();
	void UnLockMotion();
};
