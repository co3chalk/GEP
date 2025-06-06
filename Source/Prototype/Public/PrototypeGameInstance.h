#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrototypeGameInstance.generated.h"

UCLASS()
class PROTOTYPE_API UPrototypeGameInstance : public UGameInstance
{
    GENERATED_BODY()

private:
    // --- ������ ���� ---
    bool bIsClearedBasic;
    bool bIsClearedFlame;
    bool bIsClearedWater;
    bool bIsClearedElec;

    bool bIsGotBasicKey;
    bool bIsGotFlameKey;
    bool bIsGotWaterKey;
    bool bIsGotElecKey;

    // ���� ���� ������ ���� ��� ���� ����
    bool bPysGrabFeaturePermanentlyUnlocked_Session;

public:
    UPrototypeGameInstance();

    // --- ������ ���� �Լ� ---
    // MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void SetClearedMap(int MapIndex, bool isCleared);

    // KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void SetGotKey(int KeyIndex, bool isGotKey);

    // �� Ȯ���� ���� �Լ� (�ʿ�� ���)
    bool GetClearedMapState(int MapIndex) const;
    bool GetGotKeyState(int KeyIndex) const;


    // --- ���� �׷� ��� ---
    void UnlockPysGrabFeature();
    bool IsPysGrabFeatureUnlocked() const;
};