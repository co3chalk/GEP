// PrototypeGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"


UCLASS()
// PROTOTYPE_API�� ���� ������Ʈ API ��ũ�η� ����
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase // �ùٸ� �θ� Ŭ���� ��� Ȯ��
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    bool isClearedBasic = false;
    UPROPERTY(EditAnywhere)
    bool isClearedFlame = false;
    UPROPERTY(EditAnywhere)
    bool isClearedWater = false;
    UPROPERTY(EditAnywhere)
    bool isClearedElec = false;

    UPROPERTY(EditAnywhere)
    bool isGotBasickey = false;
    UPROPERTY(EditAnywhere)
    bool isGotFlamekey = false;
    UPROPERTY(EditAnywhere)
    bool isGotWaterkey = false;
    UPROPERTY(EditAnywhere)
    bool isGotEleckey = false;

    void IsClearedMap(bool isCleared);

    void IsGotKey(bool isGotKey);

private:
 
};