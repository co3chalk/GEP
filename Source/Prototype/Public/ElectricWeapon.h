#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectricWeapon.generated.h"

class AElecBullet;
class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UElectricWeapon : public UActorComponent
{
    GENERATED_BODY()

public:
    UElectricWeapon();

    UFUNCTION() void Fire();
    UFUNCTION(BlueprintPure) bool ShouldLockRotation() const { return false; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float, ELevelTick, FActorComponentTickFunction*) override;

private:
    UPROPERTY(EditAnywhere) TSubclassOf<AElecBullet> BulletClass;
    class APrototypeCharacter* OwnerChar = nullptr;
    UCameraComponent* FollowCamera = nullptr;
};
