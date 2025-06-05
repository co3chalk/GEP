#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrototypeGameInstance.generated.h"

UCLASS()
class PROTOTYPE_API UPrototypeGameInstance : public UGameInstance
{
    GENERATED_BODY()

private:
    bool bPysGrabFeaturePermanentlyUnlocked_Session; // 세션 동안 유지될 영구 잠금 해제 상태

public:
    UPrototypeGameInstance();

    void UnlockPysGrabFeature(); // 간결한 이름으로 변경
    bool IsPysGrabFeatureUnlocked() const; // 간결한 이름으로 변경
};