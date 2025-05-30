#include "PlayerUIWidget.h" // ������ ���� �̸��� �°� ����
#include "PrototypeCharacter.h"   // APrototypeCharacter ����

void UPlayerUIWidget::InitializeForPlayer(APrototypeCharacter* InPlayerCharacter)
{
    PlayerCharacterRef = InPlayerCharacter;

    if (PlayerCharacterRef)
    {
        // ��������Ʈ ���ε�
        PlayerCharacterRef->OnHPChanged.AddDynamic(this, &UPlayerUIWidget::HandleHealthChanged);

        // �ʱ� HP�� UI ������Ʈ ȣ��
        HandleHealthChanged(PlayerCharacterRef->GetCurrentHP(), PlayerCharacterRef->GetMaxHP());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidgetBase: InitializeForPlayer received a null PlayerCharacter."));
    }
}

void UPlayerUIWidget::HandleHealthChanged(int32 CurrentHP, int32 MaxHP)
{
    // �������Ʈ���� ������ OnHealthUpdated �̺�Ʈ�� ȣ��
    OnHealthUpdated(CurrentHP, MaxHP);
}