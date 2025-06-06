#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"

UCLASS()
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    APrototypeGameMode();

    virtual void BeginPlay() override;

    // --- 데이터 설정(SET)을 위한 '중개' 함수들 ---
    // MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void IsClearedMap(int MapIndex, bool isCleared);

    // KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void IsGotKey(int KeyIndex, bool isGotKey);


    // --- [추가된 부분] 데이터 확인(GET)을 위한 '중개' 함수들 ---
    UFUNCTION(BlueprintPure, Category = "Game State") // BlueprintPure는 값을 받기만 하는 함수에 사용합니다.
        bool IsMapCleared_Get(int MapIndex) const;

    UFUNCTION(BlueprintPure, Category = "Game State")
    bool HasGotKey_Get(int KeyIndex) const;


    // --- 물리 그랩 기능 관련 (기존과 동일) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|PhysicsGrab")
    bool bPysGrabEnabledForCurrentLevel;

    UFUNCTION(BlueprintCallable, Category = "Gameplay|PhysicsGrab")
    void SetPysGrabActiveForLevel(bool bEnable);

    UFUNCTION(BlueprintPure, Category = "Gameplay|PhysicsGrab")
    bool IsPysGrabActiveForLevel() const;
};