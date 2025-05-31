// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock;
class APrototypeCharacter;

UCLASS()
// YOURPROJECT_API�� ���� ������Ʈ API ��ũ�η� ���� (��: PROTOTYPE_API)
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

    // HP �ؽ�Ʈ ������Ʈ �Լ� (����)
    void UpdateHealthText(int32 CurrentHP, int32 MaxHP);

    // ���� ���� �ؽ�Ʈ ������Ʈ �Լ� (C++���� ���� ȣ�� �����ϵ��� public ����)
    void UpdateCurrentWeaponText(const FString& WeaponName);

protected:
    virtual void NativeConstruct() override;

    // HP ���� ��������Ʈ �ڵ鷯
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // ���� ���� ��������Ʈ �ڵ鷯 (���� �߰� �Ǵ� ���� ���� Ȯ��)
    UFUNCTION()
    void HandleWeaponChanged(const FString& NewWeaponName);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPDisplayText;

    // ���� ���� �̸��� ǥ���� TextBlock
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentWeaponText;

private:
    APrototypeCharacter* PlayerCharacterRef;
};