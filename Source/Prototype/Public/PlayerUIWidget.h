// PlayerUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

class UTextBlock; // UTextBlock 전방 선언
class APrototypeCharacter; // APrototypeCharacter 전방 선언

UCLASS()
// YOURPROJECT_API를 실제 프로젝트 API 매크로로 변경하세요.
class PROTOTYPE_API UPlayerUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 플레이어 캐릭터를 이 위젯에 연결하고 초기 HP를 설정합니다.
    void InitializeForPlayer(APrototypeCharacter* InPlayerCharacter);

protected:
    virtual void NativeConstruct() override;

    // APrototypeCharacter의 OnHPChanged 델리게이트에 바인딩될 함수입니다.
    UFUNCTION() // 델리게이트 바인딩을 위해 UFUNCTION() 매크로 필요
        void HandleHealthChanged(int32 CurrentHP, int32 MaxHP);

    // UMG 디자이너에서 "HPDisplayText" 이름으로 생성된 TextBlock 위젯과 연결됩니다.
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPDisplayText;

private:
    // 참조할 플레이어 캐릭터입니다.
    // 위젯이 캐릭터보다 오래 살아남는 경우가 거의 없으므로 일반 포인터도 괜찮지만,
    // 안전을 위해 TWeakObjectPtr를 고려할 수도 있습니다. 여기서는 일반 포인터를 사용합니다.
    APrototypeCharacter* PlayerCharacterRef;
};