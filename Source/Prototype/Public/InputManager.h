#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputManager.generated.h"

class UShooter;
class UFlameWeapon;
class UWaterWeapon;
class UElectricWeapon;
class APrototypeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UInputManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputManager();

    /* ---------- �Է� �ڵ鷯 ---------- */
    UFUNCTION() void HandleGrab();
    UFUNCTION() void HandleRelease();
    UFUNCTION() void HandleScrollUp();
    UFUNCTION() void HandleScrollDown();
    UFUNCTION() void HandleRightMouseDown();
    UFUNCTION() void HandleRightMouseUp();
    UFUNCTION() void HandleSwapWeapon();

protected:
    virtual void BeginPlay() override;

private:
    /* Ȱ�� ������Ʈ ���� */
    UActorComponent* GetActiveComponent() const;

    /* ---------- ���� ������Ʈ ������ ---------- */
    UPROPERTY() UShooter* Shooter = nullptr;
    UPROPERTY() UFlameWeapon* Flame = nullptr;
    UPROPERTY() UWaterWeapon* Water = nullptr;
    UPROPERTY() UElectricWeapon* Elec = nullptr;
    
    bool bUseElectric = false;
    bool bUseWater = false;
    bool bUseFlame = false;

    bool bLevelCleared = false;

    APrototypeCharacter* OwnerChar = nullptr;

};
