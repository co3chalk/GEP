// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock;
class APrototypeCharacter;

UCLASS()
// YOURPROJECT_API를 실제 프로젝트 API 매크로로 변경 (예: PROTOTYPE_API)
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

    // HP 텍스트 업데이트 함수 (기존)
    void UpdateHealthText(int32 CurrentHP, int32 MaxHP);

    // 현재 무기 텍스트 업데이트 함수 (C++에서 직접 호출 가능하도록 public 유지)
    void UpdateCurrentWeaponText(const FString& WeaponName);

protected:
    virtual void NativeConstruct() override;

    // HP 변경 델리게이트 핸들러
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // 무기 변경 델리게이트 핸들러 (새로 추가 또는 이전 제안 확인)
    UFUNCTION()
    void HandleWeaponChanged(const FString& NewWeaponName);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPDisplayText;

    // 현재 무기 이름을 표시할 TextBlock
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentWeaponText;

private:
    APrototypeCharacter* PlayerCharacterRef;
};