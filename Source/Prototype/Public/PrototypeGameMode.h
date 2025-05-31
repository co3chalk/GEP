// PrototypeGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"

class UPlayerUIWidget; // UPlayerUIWidget ���� ����
class APrototypeCharacter; // APrototypeCharacter ���� ���� (�ʿ��)

UCLASS()
// PROTOTYPE_API�� ���� ������Ʈ API ��ũ�η� ����
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase // �ùٸ� �θ� Ŭ���� ��� Ȯ��
{
    GENERATED_BODY()

public:
    // �������Ʈ �����Ϳ��� PlayerUIWidget Ŭ������ ������ �� �ֵ��� ��
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPlayerUIWidget> PlayerHealthUIClass;

protected:
    virtual void BeginPlay() override;

private:
    // ������ PlayerUIWidget �ν��Ͻ��� ����
    UPROPERTY()
    UPlayerUIWidget* PlayerHealthUIInstance;
};