#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputManager.generated.h"

class UShooter;
//class UFireComponent;
class UWaterWeapon;
class UElectricWeapon;
class APrototypeCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UInputManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputManager();

    /* ---------- 입력 핸들러 ---------- */
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
    /* 활성 컴포넌트 판정 */
    UActorComponent* GetActiveComponent() const;

    /* ---------- 의존 컴포넌트 포인터 ---------- */
    UPROPERTY() UShooter* Shooter = nullptr;
    //UPROPERTY() UFireComponent*  Fire  = nullptr;
    UPROPERTY() UWaterWeapon* Water = nullptr;
    UPROPERTY() UElectricWeapon* Elec = nullptr;
    
    bool bUseElectric = false;
    bool bUseWater = false;

    bool bLevelCleared = false;

    APrototypeCharacter* OwnerChar = nullptr;

};
