#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h" // 생성한 파일 이름에 맞게 변경

class APrototypeCharacter; // 전방 선언

UCLASS()
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    // 플레이어 캐릭터에 대한 참조
    UPROPERTY(BlueprintReadOnly, Category = "Setup")
    APrototypeCharacter* PlayerCharacterRef;

    // 블루프린트에서 UI 업데이트 로직을 구현할 이벤트
    UFUNCTION(BlueprintImplementableEvent, Category = "UI Update")
    void OnHealthUpdated(int32 CurrentHP, int32 MaxHP);

    // 캐릭터의 OnHPChanged 델리게이트에 바인딩될 함수
    UFUNCTION()
    void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

public:
    // 블루프린트에서 호출하여 캐릭터 참조를 설정하고 델리게이트를 바인딩하는 함수
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);
};