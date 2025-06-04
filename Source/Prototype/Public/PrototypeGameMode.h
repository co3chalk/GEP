// PrototypeGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"


UCLASS()
// PROTOTYPE_API는 실제 프로젝트 API 매크로로 변경
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase // 올바른 부모 클래스 상속 확인
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