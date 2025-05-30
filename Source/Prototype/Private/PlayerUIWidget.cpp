#include "PlayerUIWidget.h" // 생성한 파일 이름에 맞게 변경
#include "PrototypeCharacter.h"   // APrototypeCharacter 참조

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter;

    if (PlayerCharacterRef)
    {
        // 델리게이트 바인딩
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged);

        // 초기 HP로 UI 업데이트 호출
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidgetBase: InitializeForPlayer received a null PlayerCharacter."));
    }
}

void UPlayerUIWidget::HandleHealthChanged(int32 CurrentHP, int32 MaxHP)
{
    // 블루프린트에서 구현된 OnHealthUpdated 이벤트를 호출
    OnHealthUpdated(CurrentHP, MaxHP);
}