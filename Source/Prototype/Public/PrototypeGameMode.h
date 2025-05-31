// PrototypeGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"

class UPlayerUIWidget; // UPlayerUIWidget 전방 선언
class APrototypeCharacter; // APrototypeCharacter 전방 선언 (필요시)

UCLASS()
// PROTOTYPE_API는 실제 프로젝트 API 매크로로 변경
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase // 올바른 부모 클래스 상속 확인
{
    GENERATED_BODY()

public:
    // 블루프린트 에디터에서 PlayerUIWidget 클래스를 선택할 수 있도록 함
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPlayerUIWidget> PlayerHealthUIClass;

protected:
    virtual void BeginPlay() override;

private:
    // 생성된 PlayerUIWidget 인스턴스를 저장
    UPROPERTY()
    UPlayerUIWidget* PlayerHealthUIInstance;
};