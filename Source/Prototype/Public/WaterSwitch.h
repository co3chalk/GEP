// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
UCLASS()
class PROTOTYPE_API AWaterSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterSwitch();
	UFUNCTION(BlueprintPure) bool IsPressed() const { return OverlapCount > 0; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    UPROPERTY(VisibleAnywhere, Category = "Switch")
    UStaticMeshComponent* SwitchMesh = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Switch")
    UBoxComponent* Trigger = nullptr;

    UPROPERTY(EditAnywhere, Category = "Switch")
    TArray<AActor*> PoweredTargets;

    int32 OverlapCount = 0;

    UFUNCTION()
    void OnBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEnd(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

    void SetTargetsPowered(bool bOn);
};
