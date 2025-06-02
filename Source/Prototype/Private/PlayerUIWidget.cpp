// PlayerUIWidget.cpp
#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "PrototypeCharacter.h" // APrototypeCharacter 정의 포함

void UPlayerUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // PlayerCharacterRef가 아직 없을 수 있으므로, 초기 텍스트 설정
    UpdateHealthText(0, 0); // 예시: 초기값 또는 "N/A"
    UpdateCurrentWeaponText(TEXT("N/A")); //
    UpdateAllEnergyDisplays(); // 에너지 텍스트도 초기화
}

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter; //

    if (PlayerCharacterRef)
    {
        // HP 델리게이트 바인딩
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged); //
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP()); // 초기값 설정

        // 무기 변경 델리게이트 바인딩
        PlayerCharacterRef->OnWeaponChanged.AddDynamic(this, &UPlayerUIWidget::HandleWeaponChanged); //
        UpdateCurrentWeaponText(PlayerCharacterRef->GetCurrentWeaponName()); // 초기값 설정

        // 에너지 변경 델리게이트 바인딩 (새로 추가)
        PlayerCharacterRef->OnBasicEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleBasicEnergyChanged); //
        PlayerCharacterRef->OnFlameEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleFlameEnergyChanged); //
        PlayerCharacterRef->OnWaterEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleWaterEnergyChanged); //
        PlayerCharacterRef->OnElectricEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleElectricEnergyChanged); //

        UpdateAllEnergyDisplays(); // 캐릭터로부터 현재 에너지 값으로 UI 초기 업데이트
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: InitializeForPlayer received a null PlayerCharacter. Setting UI to N/A.")); //
        // 캐릭터가 없으면 UI를 기본값/N/A로 설정
        UpdateHealthText(0, 0); // 혹은 적절한 N/A 값
        UpdateCurrentWeaponText(TEXT("N/A")); //
        UpdateAllEnergyDisplays(); // 에너지 텍스트도 N/A로 설정
    }
}

// UpdateHealthText 함수 구현 (이전 코드에서 제공된 내용, LNK2019 해결을 위해 필요)
void UPlayerUIWidget::UpdateHealthText(int32 CurrentHP, int32 MaxHP)
{
    if (HPDisplayText != nullptr)
    {
        FString HealthString = FString::Printf(TEXT("HP : %d / %d"), CurrentHP, MaxHP);
        HPDisplayText->SetText(FText::FromString(HealthString));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget: HPDisplayText is NULL in UpdateHealthText. Check BindWidget."));
    }
}

void UPlayerUIWidget::HandleHealthChanged(int32 CurrentHP, int32 MaxHP)
{
    // UpdateHealthText 함수를 직접 호출 (이전 코드에서는 HPDisplayText 직접 업데이트)
    UpdateHealthText(CurrentHP, MaxHP);
}

void UPlayerUIWidget::HandleWeaponChanged(const FString& NewWeaponName)
{
    UpdateCurrentWeaponText(NewWeaponName); //
}

void UPlayerUIWidget::UpdateCurrentWeaponText(const FString& WeaponName)
{
    if (CurrentWeaponText != nullptr) //
    {
        FString WeaponDisplayString = FString::Printf(TEXT("Weapon : %s"), *WeaponName); //
        CurrentWeaponText->SetText(FText::FromString(WeaponDisplayString)); //
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget: CurrentWeaponText is NULL. Check BindWidget in Blueprint.")); //
    }
}

// --- 에너지 UI 업데이트 함수 구현 ---

void UPlayerUIWidget::UpdateAllEnergyDisplays()
{
    if (PlayerCharacterRef) //
    {
        // 캐릭터에서 직접 현재 에너지 값을 가져와서 각 핸들러 호출 또는 직접 TextBlock 업데이트
        HandleBasicEnergyChanged(PlayerCharacterRef->GetBasicEnergy()); //
        HandleFlameEnergyChanged(PlayerCharacterRef->GetFlameEnergy()); //
        HandleWaterEnergyChanged(PlayerCharacterRef->GetWaterEnergy()); //
        HandleElectricEnergyChanged(PlayerCharacterRef->GetElectricEnergy()); //
    }
    else
    {
        // PlayerCharacterRef가 null이면 "N/A" 또는 기본값으로 설정
        if (BasicEnergyCountText) BasicEnergyCountText->SetText(FText::FromString(TEXT("Basic: N/A"))); //
        if (FlameEnergyCountText) FlameEnergyCountText->SetText(FText::FromString(TEXT("Flame: N/A"))); //
        if (WaterEnergyCountText) WaterEnergyCountText->SetText(FText::FromString(TEXT("Water: N/A"))); //
        if (ElectricEnergyCountText) ElectricEnergyCountText->SetText(FText::FromString(TEXT("Electric: N/A"))); 
    }
}

// 에너지 핸들러 함수 구현 추가
void UPlayerUIWidget::HandleBasicEnergyChanged(int32 NewEnergyCount)
{
    if (BasicEnergyCountText != nullptr) //
    {
        FString EnergyStr = FString::Printf(TEXT("Basic: %d"), NewEnergyCount); //
        BasicEnergyCountText->SetText(FText::FromString(EnergyStr)); //
    }
    // else { UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: BasicEnergyCountText is NULL.")); } // 필요시 로그 활성화
}

void UPlayerUIWidget::HandleFlameEnergyChanged(int32 NewEnergyCount)
{
    if (FlameEnergyCountText != nullptr) //
    {
        FString EnergyStr = FString::Printf(TEXT("Flame: %d"), NewEnergyCount); //
        FlameEnergyCountText->SetText(FText::FromString(EnergyStr)); //
    }
    // else { UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: FlameEnergyCountText is NULL.")); }
}

void UPlayerUIWidget::HandleWaterEnergyChanged(int32 NewEnergyCount)
{
    if (WaterEnergyCountText != nullptr) //
    {
        FString EnergyStr = FString::Printf(TEXT("Water: %d"), NewEnergyCount); //
        WaterEnergyCountText->SetText(FText::FromString(EnergyStr)); //
    }
    // else { UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: WaterEnergyCountText is NULL.")); }
}

void UPlayerUIWidget::HandleElectricEnergyChanged(int32 NewEnergyCount)
{
    if (ElectricEnergyCountText != nullptr) //
    {
        FString EnergyStr = FString::Printf(TEXT("Electric: %d"), NewEnergyCount); //
        ElectricEnergyCountText->SetText(FText::FromString(EnergyStr)); //
    }
    // else { UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: ElectricEnergyCountText is NULL.")); }
}