// InputManager.h
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

    /* ---------- 입력 핸들러 ---------- */
    UFUNCTION() void HandleGrab(); //
    UFUNCTION() void HandleRelease(); //
    UFUNCTION() void HandleScrollUp(); //
    UFUNCTION() void HandleScrollDown(); //
    UFUNCTION() void HandleRightMouseDown(); //
    UFUNCTION() void HandleRightMouseUp(); //
    UFUNCTION() void HandleSwapWeapon(); //

    // 현재 활성화된 무기 컴포넌트를 반환하는 public getter (private에서 public으로 변경)
    UFUNCTION(BlueprintPure, Category = "Weapon")
    UActorComponent* GetActiveComponent() const; //

    // 각 무기 사용 여부를 반환하는 public getter (새로 추가)
    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsElectricWeaponActive() const { return bUseElectric; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsWaterWeaponActive() const { return bUseWater; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsFlameWeaponActive() const { return bUseFlame; }


protected:
    virtual void BeginPlay() override; //

private:

    /* ---------- 의존 컴포넌트 포인터 ---------- */
    UPROPERTY() UShooter* Shooter = nullptr; //
    UPROPERTY() UFlameWeapon* Flame = nullptr; //
    UPROPERTY() UWaterWeapon* Water = nullptr; //
    UPROPERTY() UElectricWeapon* Elec = nullptr; //

    // 이 변수들은 이제 public getter를 통해 접근합니다.
    bool bUseElectric = false; //
    bool bUseWater = false; //
    bool bUseFlame = false; //

    bool bLevelCleared = false; //

    APrototypeCharacter* OwnerChar = nullptr; //
};