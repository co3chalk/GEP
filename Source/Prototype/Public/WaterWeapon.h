#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterWeapon.generated.h"

class AWaterBullet;
class APrototypeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UWaterWeapon : public UActorComponent
{
    GENERATED_BODY()

public:
    UWaterWeapon();

    void StartFire();
    void StopFire();

protected:
    virtual void BeginPlay() override;
    
public:
    void SpawnWater();

    UPROPERTY(EditAnywhere, Category = "Water")
    TSubclassOf<AWaterBullet> WaterBulletClass;

    UPROPERTY(EditAnywhere, Category = "Water")
    float FireInterval;

    APrototypeCharacter* OwnerCharacter;

    FTimerHandle FireTimerHandle;

 
};
