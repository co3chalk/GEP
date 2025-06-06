#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrototypeGameInstance.generated.h"

UCLASS()
class PROTOTYPE_API UPrototypeGameInstance : public UGameInstance
{
    GENERATED_BODY()

private:
    // --- 데이터 변수 ---
    bool bIsClearedBasic;
    bool bIsClearedFlame;
    bool bIsClearedWater;
    bool bIsClearedElec;

    bool bIsGotBasicKey;
    bool bIsGotFlameKey;
    bool bIsGotWaterKey;
    bool bIsGotElecKey;

    // 세션 동안 유지될 영구 잠금 해제 상태
    bool bPysGrabFeaturePermanentlyUnlocked_Session;

public:
    UPrototypeGameInstance();

    // --- 데이터 제어 함수 ---
    // MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void SetClearedMap(int MapIndex, bool isCleared);

    // KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void SetGotKey(int KeyIndex, bool isGotKey);

    // 값 확인을 위한 함수 (필요시 사용)
    bool GetClearedMapState(int MapIndex) const;
    bool GetGotKeyState(int KeyIndex) const;


    // --- 물리 그랩 기능 ---
    void UnlockPysGrabFeature();
    bool IsPysGrabFeatureUnlocked() const;
};