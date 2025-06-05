#include "PrototypeGameInstance.h"

UPrototypeGameInstance::UPrototypeGameInstance()
{
    bPysGrabFeaturePermanentlyUnlocked_Session = false; // ���� ���� �� �⺻�� ���
}

void UPrototypeGameInstance::UnlockPysGrabFeature()
{
    if (!bPysGrabFeaturePermanentlyUnlocked_Session)
    {
        bPysGrabFeaturePermanentlyUnlocked_Session = true;
        UE_LOG(LogTemp, Warning, TEXT("GameInstance: ���� �׷� ����� ���� ���� ���������� ��� �����Ǿ����ϴ�!"));
        // TODO: �ʿ��ϴٸ� ���⿡ SaveGame ������ �߰��Ͽ� ��ũ�� ����
    }
}

bool UPrototypeGameInstance::IsPysGrabFeatureUnlocked() const
{
    return bPysGrabFeaturePermanentlyUnlocked_Session;
}