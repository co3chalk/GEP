// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock;
class APrototypeCharacter;

UCLASS()
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget // YOURPROJECT_API�� ���� ������Ʈ API ��ũ�η� ���� (��: PROTOTYPE_API)
{
    GENERATED_BODY()

public:
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

    // HP �ؽ�Ʈ ������Ʈ �Լ�
    void UpdateHealthText(int32 CurrentHP, int32 MaxHP);

    // ���� ���� �ؽ�Ʈ ������Ʈ �Լ�
    void UpdateCurrentWeaponText(const FString& WeaponName);

    // ��� ������ ǥ�ø� ������Ʈ�ϴ� �Լ� (���� �߰�)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAllEnergyDisplays();

protected:
    virtual void NativeConstruct() override;

    // HP ���� ��������Ʈ �ڵ鷯
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // ���� ���� ��������Ʈ �ڵ鷯
    UFUNCTION()
    void HandleWeaponChanged(const FString& NewWeaponName);

    /* --- ������ ���� ��������Ʈ �ڵ鷯 (���� �߰�) --- */
    UFUNCTION()
    void HandleBasicEnergyChanged(int32 NewEnergyCount);

    UFUNCTION()
    void HandleFlameEnergyChanged(int32 NewEnergyCount);

    UFUNCTION()
    void HandleWaterEnergyChanged(int32 NewEnergyCount);

    UFUNCTION()
    void HandleElectricEnergyChanged(int32 NewEnergyCount);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPDisplayText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentWeaponText;

    /* --- ������ ���� ǥ�ÿ� TextBlock (���� �߰�, �������Ʈ���� ���ε� �ʿ�) --- */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BasicEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* FlameEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* WaterEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ElectricEnergyCountText;

private:
    UPROPERTY() // PlayerCharacterRef�� GC���� �ʵ��� UPROPERTY() �߰�
        APrototypeCharacter* PlayerCharacterRef;
};