#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrototypeGameInstance.generated.h"

UCLASS()
class PROTOTYPE_API UPrototypeGameInstance : public UGameInstance
{
    GENERATED_BODY()

private:
    bool bPysGrabFeaturePermanentlyUnlocked_Session; // ���� ���� ������ ���� ��� ���� ����

public:
    UPrototypeGameInstance();

    void UnlockPysGrabFeature(); // ������ �̸����� ����
    bool IsPysGrabFeatureUnlocked() const; // ������ �̸����� ����
};