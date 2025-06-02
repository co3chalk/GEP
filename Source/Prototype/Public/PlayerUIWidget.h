// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock;
class APrototypeCharacter;

UCLASS()
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget // YOURPROJECT_API를 실제 프로젝트 API 매크로로 변경 (예: PROTOTYPE_API)
{
    GENERATED_BODY()

public:
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

    // HP 텍스트 업데이트 함수
    void UpdateHealthText(int32 CurrentHP, int32 MaxHP);

    // 현재 무기 텍스트 업데이트 함수
    void UpdateCurrentWeaponText(const FString& WeaponName);

    // 모든 에너지 표시를 업데이트하는 함수 (새로 추가)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAllEnergyDisplays();

protected:
    virtual void NativeConstruct() override;

    // HP 변경 델리게이트 핸들러
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // 무기 변경 델리게이트 핸들러
    UFUNCTION()
    void HandleWeaponChanged(const FString& NewWeaponName);

    /* --- 에너지 변경 델리게이트 핸들러 (새로 추가) --- */
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

    /* --- 에너지 개수 표시용 TextBlock (새로 추가, 블루프린트에서 바인딩 필요) --- */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BasicEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* FlameEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* WaterEnergyCountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ElectricEnergyCountText;

private:
    UPROPERTY() // PlayerCharacterRef가 GC되지 않도록 UPROPERTY() 추가
        APrototypeCharacter* PlayerCharacterRef;
};