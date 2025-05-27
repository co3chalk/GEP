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

	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* RopeConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	float WeightThreshold = 10.0f;


	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	float CurrentWeight = 0.0f;
};
