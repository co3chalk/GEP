// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock; // UTextBlock ���� ����
class APrototypeCharacter; // APrototypeCharacter ���� ����

UCLASS()
// YOURPROJECT_API�� ���� ������Ʈ API ��ũ�η� �����ϼ���.
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // �÷��̾� ĳ���͸� �� ������ �����ϰ� �ʱ� HP�� �����մϴ�.
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

protected:
    virtual void NativeConstruct() override;

    // APrototypeCharacter�� OnHPChanged ��������Ʈ�� ���ε��� �Լ��Դϴ�.
    UFUNCTION() // ��������Ʈ ���ε��� ���� UFUNCTION() ��ũ�� �ʿ�
        void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // UMG �����̳ʿ��� "HPDisplayText" �̸����� ������ TextBlock ������ ����˴ϴ�.
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPDisplayText;

private:
    // ������ �÷��̾� ĳ�����Դϴ�.
    // ������ ĳ���ͺ��� ���� ��Ƴ��� ��찡 ���� �����Ƿ� �Ϲ� �����͵� ��������,
    // ������ ���� TWeakObjectPtr�� ����� ���� �ֽ��ϴ�. ���⼭�� �Ϲ� �����͸� ����մϴ�.
    APrototypeCharacter* PlayerCharacterRef;
};