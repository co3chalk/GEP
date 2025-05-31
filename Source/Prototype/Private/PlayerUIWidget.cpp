// PlayerUIWidget.cpp
#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "PrototypeCharacter.h" // APrototypeCharacter�� ���Ǹ� ���� ����

void UPlayerUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // �ʱ⿡�� HPDisplayText�� �������� �ʾ��� �� �����Ƿ�,
    // ���� �ʱ�ȭ�� InitializeForPlayer���� �����մϴ�.
}

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter;

    if (PlayerCharacterRef)
    {
        // �÷��̾� ĳ������ OnHPChanged ��������Ʈ�� �츮 �Լ��� ���ε��մϴ�.
        // �� ��������Ʈ�� APrototypeCharacter.h�� ����Ǿ� �־�� �մϴ�.
        // (��: DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, int32, CurrentHP, int32, MaxHP);)
        // (UPROPERTY(BlueprintAssignable) FOnHPChangedDelegate OnHPChanged;)
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged); //

        // �ʱ� HP�� UI�� �ѹ� ������Ʈ�մϴ�.
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP()); //
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: InitializeForPlayer received a null PlayerCharacter."));
        if (HPDisplayText) // �÷��̾ ������ �⺻ �ؽ�Ʈ ����
        {
            HPDisplayText->SetText(FText::FromString(TEXT("HP : N/A")));
        }
    }
}

void UPlayerUIWidget::HandleHealthChanged(int32 CurrentHP, int32 MaxHP)
{
    if (HPDisplayText != nullptr) //
    {
        FString HealthString = FString::Printf(TEXT("HP  :  %d / %d"), CurrentHP, MaxHP); //
        HPDisplayText->SetText(FText::FromString(HealthString)); //
    }
    else //
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: HPDisplayText is not valid in HandleHealthChanged.")); //
    }
}