#include "PrototypeGameInstance.h"

UPrototypeGameInstance::UPrototypeGameInstance()
{
    bPysGrabFeaturePermanentlyUnlocked_Session = false; // 게임 시작 시 기본은 잠김
}

void UPrototypeGameInstance::UnlockPysGrabFeature()
{
    if (!bPysGrabFeaturePermanentlyUnlocked_Session)
    {
        bPysGrabFeaturePermanentlyUnlocked_Session = true;
        UE_LOG(LogTemp, Warning, TEXT("GameInstance: 물리 그랩 기능이 세션 동안 영구적으로 잠금 해제되었습니다!"));
        // TODO: 필요하다면 여기에 SaveGame 로직을 추가하여 디스크에 저장
    }
}

bool UPrototypeGameInstance::IsPysGrabFeatureUnlocked() const
{
    return bPysGrabFeaturePermanentlyUnlocked_Session;
}