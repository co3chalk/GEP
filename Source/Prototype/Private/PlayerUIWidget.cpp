// PlayerUIWidget.cpp
#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "PrototypeCharacter.h" // APrototypeCharacter의 정의를 위해 포함

void UPlayerUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // 초기에는 HPDisplayText가 설정되지 않았을 수 있으므로,
    // 실제 초기화는 InitializeForPlayer에서 수행합니다.
}

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter;

    if (PlayerCharacterRef)
    {
        // 플레이어 캐릭터의 OnHPChanged 델리게이트에 우리 함수를 바인딩합니다.
        // 이 델리게이트는 APrototypeCharacter.h에 선언되어 있어야 합니다.
        // (예: DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, int32, CurrentHP, int32, MaxHP);)
        // (UPROPERTY(BlueprintAssignable) FOnHPChangedDelegate OnHPChanged;)
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged); //

        // 초기 HP로 UI를 한번 업데이트합니다.
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP()); //
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget: InitializeForPlayer received a null PlayerCharacter."));
        if (HPDisplayText) // 플레이어가 없으면 기본 텍스트 설정
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