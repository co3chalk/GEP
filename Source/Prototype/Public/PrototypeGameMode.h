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

    // --- ������ ����(SET)�� ���� '�߰�' �Լ��� ---
    // MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void IsClearedMap(int MapIndex, bool isCleared);

    // KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
    void IsGotKey(int KeyIndex, bool isGotKey);


    // --- [�߰��� �κ�] ������ Ȯ��(GET)�� ���� '�߰�' �Լ��� ---
    UFUNCTION(BlueprintPure, Category = "Game State") // BlueprintPure�� ���� �ޱ⸸ �ϴ� �Լ��� ����մϴ�.
        bool IsMapCleared_Get(int MapIndex) const;

    UFUNCTION(BlueprintPure, Category = "Game State")
    bool HasGotKey_Get(int KeyIndex) const;


    // --- ���� �׷� ��� ���� (������ ����) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|PhysicsGrab")
    bool bPysGrabEnabledForCurrentLevel;

    UFUNCTION(BlueprintCallable, Category = "Gameplay|PhysicsGrab")
    void SetPysGrabActiveForLevel(bool bEnable);

    UFUNCTION(BlueprintPure, Category = "Gameplay|PhysicsGrab")
    bool IsPysGrabActiveForLevel() const;
};