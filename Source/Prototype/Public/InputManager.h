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

    /* ---------- �Է� �ڵ鷯 ---------- */
    UFUNCTION() void HandleGrab(); //
    UFUNCTION() void HandleRelease(); //
    UFUNCTION() void HandleScrollUp(); //
    UFUNCTION() void HandleScrollDown(); //
    UFUNCTION() void HandleRightMouseDown(); //
    UFUNCTION() void HandleRightMouseUp(); //
    UFUNCTION() void HandleSwapWeapon(); //

    // ���� Ȱ��ȭ�� ���� ������Ʈ�� ��ȯ�ϴ� public getter (private���� public���� ����)
    UFUNCTION(BlueprintPure, Category = "Weapon")
    UActorComponent* GetActiveComponent() const; //

    // �� ���� ��� ���θ� ��ȯ�ϴ� public getter (���� �߰�)
    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsElectricWeaponActive() const { return bUseElectric; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsWaterWeaponActive() const { return bUseWater; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsFlameWeaponActive() const { return bUseFlame; }


protected:
    virtual void BeginPlay() override; //

private:

    /* ---------- ���� ������Ʈ ������ ---------- */
    UPROPERTY() UShooter* Shooter = nullptr; //
    UPROPERTY() UFlameWeapon* Flame = nullptr; //
    UPROPERTY() UWaterWeapon* Water = nullptr; //
    UPROPERTY() UElectricWeapon* Elec = nullptr; //

    // �� �������� ���� public getter�� ���� �����մϴ�.
    bool bUseElectric = false; //
    bool bUseWater = false; //
    bool bUseFlame = false; //

    bool bLevelCleared = false; //

    APrototypeCharacter* OwnerChar = nullptr; //
};