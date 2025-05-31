// PlayerUIWidget.cpp
#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "PrototypeCharacter.h" // APrototypeCharacter 정의 포함

void UPlayerUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter;

    if (PlayerCharacterRef)
    {
        // HP 델리게이트 바인딩
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged);
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP());

        // 무기 변경 델리게이트 바인딩 (중요)
        PlayerCharacterRef->OnWeaponChanged.AddDynamic(this, &UPlayerUIWidget::HandleWeaponChanged); //
        // 초기 무기 이름으로 UI 업데이트
        UpdateCurrentWeaponText(PlayerCharacterRef->GetCurrentWeaponName()); //
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: InitializeForPlayer received a null PlayerCharacter."));
        if (HPDisplayText)
        {
            HPDisplayText->SetText(FText::FromString(TEXT("HP: N/A")));
        }
        if (CurrentWeaponText)
        {
            CurrentWeaponText->SetText(FText::FromString(TEXT("Weapon: N/A")));
        }
    }
}

void UPlayerUIWidget::HandleHealthChanged(int32 CurrentHP, int32 MaxHP)
{
    if (HPDisplayText != nullptr)
    {
        FString HealthString = FString::Printf(TEXT("HP : %d / %d"), CurrentHP, MaxHP);
        HPDisplayText->SetText(FText::FromString(HealthString));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget: HPDisplayText is NULL in HandleHealthChanged."));
    }
}

void UPlayerUIWidget::HandleWeaponChanged(const FString& NewWeaponName)
{
    UpdateCurrentWeaponText(NewWeaponName);
}

void UPlayerUIWidget::UpdateCurrentWeaponText(const FString& WeaponName)
{
    if (CurrentWeaponText != nullptr)
    {
        FString WeaponDisplayString = FString::Printf(TEXT("Weapon : %s"), *WeaponName);
        CurrentWeaponText->SetText(FText::FromString(WeaponDisplayString));
        UE_LOG(LogTemp, Log, TEXT("PlayerUIWidget: CurrentWeaponText updated to: %s"), *WeaponDisplayString);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget: CurrentWeaponText is NULL. Check BindWidget in Blueprint."));
    }
}