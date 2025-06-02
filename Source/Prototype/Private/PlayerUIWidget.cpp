// PlayerUIWidget.cpp
#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "PrototypeCharacter.h" // APrototypeCharacter ���� ����

void UPlayerUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // PlayerCharacterRef�� ���� ���� �� �����Ƿ�, �ʱ� �ؽ�Ʈ ����
    UpdateHealthText(0, 0); // ����: �ʱⰪ �Ǵ� "N/A"
    UpdateCurrentWeaponText(TEXT("N/A")); //
    UpdateAllEnergyDisplays(); // ������ �ؽ�Ʈ�� �ʱ�ȭ
}

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter; //

    if (PlayerCharacterRef)
    {
        // HP ��������Ʈ ���ε�
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged); //
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP()); // �ʱⰪ ����

        // ���� ���� ��������Ʈ ���ε�
        PlayerCharacterRef->OnWeaponChanged.AddDynamic(this, &UPlayerUIWidget::HandleWeaponChanged); //
        UpdateCurrentWeaponText(PlayerCharacterRef->GetCurrentWeaponName()); // �ʱⰪ ����

        // ������ ���� ��������Ʈ ���ε� (���� �߰�)
        PlayerCharacterRef->OnBasicEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleBasicEnergyChanged); //
        PlayerCharacterRef->OnFlameEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleFlameEnergyChanged); //
        PlayerCharacterRef->OnWaterEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleWaterEnergyChanged); //
        PlayerCharacterRef->OnElectricEnergyChanged.AddDynamic(this, &UPlayerUIWidget::HandleElectricEnergyChanged); //

        UpdateAllEnergyDisplays(); // ĳ���ͷκ��� ���� ������ ������ UI �ʱ� ������Ʈ
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: InitializeForPlayer received a null PlayerCharacter. Setting UI to N/A.")); //
        // ĳ���Ͱ� ������ UI�� �⺻��/N/A�� ����
        UpdateHealthText(0, 0); // Ȥ�� ������ N/A ��
        UpdateCurrentWeaponText(TEXT("N/A")); //
        UpdateAllEnergyDisplays(); // ������ �ؽ�Ʈ�� N/A�� ����
    }
}

// UpdateHealthText �Լ� ���� (���� �ڵ忡�� ������ ����, LNK2019 �ذ��� ���� �ʿ�)
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
    // UpdateHealthText �Լ��� ���� ȣ�� (���� �ڵ忡���� HPDisplayText ���� ������Ʈ)
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

// --- ������ UI ������Ʈ �Լ� ���� ---

void UPlayerUIWidget::UpdateAllEnergyDisplays()
{
    if (PlayerCharacterRef) //
    {
        // ĳ���Ϳ��� ���� ���� ������ ���� �����ͼ� �� �ڵ鷯 ȣ�� �Ǵ� ���� TextBlock ������Ʈ
        HandleBasicEnergyChanged(PlayerCharacterRef->GetBasicEnergy()); //
        HandleFlameEnergyChanged(PlayerCharacterRef->GetFlameEnergy()); //
        HandleWaterEnergyChanged(PlayerCharacterRef->GetWaterEnergy()); //
        HandleElectricEnergyChanged(PlayerCharacterRef->GetElectricEnergy()); //
    }
    else
    {
        // PlayerCharacterRef�� null�̸� "N/A" �Ǵ� �⺻������ ����
        if (BasicEnergyCountText) BasicEnergyCountText->SetText(FText::FromString(TEXT("Basic: N/A"))); //
        if (FlameEnergyCountText) FlameEnergyCountText->SetText(FText::FromString(TEXT("Flame: N/A"))); //
        if (WaterEnergyCountText) WaterEnergyCountText->SetText(FText::FromString(TEXT("Water: N/A"))); //
        if (ElectricEnergyCountText) ElectricEnergyCountText->SetText(FText::FromString(TEXT("Electric: N/A"))); 
    }
}

// ������ �ڵ鷯 �Լ� ���� �߰�
void UPlayerUIWidget::HandleBasicEnergyChanged(int32 NewEnergyCount)
{
    if (BasicEnergyCountText != nullptr) //
    {
        FString EnergyStr = FString::Printf(TEXT("Basic: %d"), NewEnergyCount); //
        BasicEnergyCountText->SetText(FText::FromString(EnergyStr)); //
    }
    // else { UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: BasicEnergyCountText is NULL.")); } // �ʿ�� �α� Ȱ��ȭ
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