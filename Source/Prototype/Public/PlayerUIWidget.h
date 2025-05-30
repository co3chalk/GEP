#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h" // ������ ���� �̸��� �°� ����

class APrototypeCharacter; // ���� ����

UCLASS()
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    // �÷��̾� ĳ���Ϳ� ���� ����
    UPROPERTY(BlueprintReadOnly, Category = "Setup")
    APrototypeCharacter* PlayerCharacterRef;

    // �������Ʈ���� UI ������Ʈ ������ ������ �̺�Ʈ
    UFUNCTION(BlueprintImplementableEvent, Category = "UI Update")
    void OnHealthUpdated(int32 CurrentHP, int32 MaxHP);

    // ĳ������ OnHPChanged ��������Ʈ�� ���ε��� �Լ�
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

public:
    // �������Ʈ���� ȣ���Ͽ� ĳ���� ������ �����ϰ� ��������Ʈ�� ���ε��ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);
};