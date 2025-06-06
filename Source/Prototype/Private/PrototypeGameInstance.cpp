#include "PrototypeGameInstance.h"

UPrototypeGameInstance::UPrototypeGameInstance()
{
    // ���� �ʱ�ȭ
    bIsClearedBasic = false;
    bIsClearedFlame = false;
    bIsClearedWater = false;
    bIsClearedElec = false;

    bIsGotBasicKey = false;
    bIsGotFlameKey = false;
    bIsGotWaterKey = false;
    bIsGotElecKey = false;

    bPysGrabFeaturePermanentlyUnlocked_Session = false;
}

// MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
void UPrototypeGameInstance::SetClearedMap(int MapIndex, bool isCleared)
{
    switch (MapIndex)
    {
    case 0: bIsClearedBasic = isCleared; break;
    case 1: bIsClearedFlame = isCleared; break;
    case 2: bIsClearedWater = isCleared; break;
    case 3: bIsClearedElec = isCleared; break;
    default: break;
    }
}

// KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
void UPrototypeGameInstance::SetGotKey(int KeyIndex, bool isGotKey)
{
    switch (KeyIndex)
    {
    case 0: bIsGotBasicKey = isGotKey; break;
    case 1: bIsGotFlameKey = isGotKey; break;
    case 2: bIsGotWaterKey = isGotKey; break;
    case 3: bIsGotElecKey = isGotKey; break;
    default: break;
    }
}

bool UPrototypeGameInstance::GetClearedMapState(int MapIndex) const
{
    switch (MapIndex)
    {
    case 0: return bIsClearedBasic;
    case 1: return bIsClearedFlame;
    case 2: return bIsClearedWater;
    case 3: return bIsClearedElec;
    default: return false;
    }
}

bool UPrototypeGameInstance::GetGotKeyState(int KeyIndex) const
{
    switch (KeyIndex)
    {
    case 0: return bIsGotBasicKey;
    case 1: return bIsGotFlameKey;
    case 2: return bIsGotWaterKey;
    case 3: return bIsGotElecKey;
    default: return false;
    }
}

void UPrototypeGameInstance::UnlockPysGrabFeature()
{
    if (!bPysGrabFeaturePermanentlyUnlocked_Session)
    {
        bPysGrabFeaturePermanentlyUnlocked_Session = true;
        UE_LOG(LogTemp, Warning, TEXT("GameInstance: ���� �׷� ����� ���� ���� ���������� ��� �����Ǿ����ϴ�!"));
    }
}

bool UPrototypeGameInstance::IsPysGrabFeatureUnlocked() const
{
    return bPysGrabFeaturePermanentlyUnlocked_Session;
}