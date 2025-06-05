// PrototypeGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"


UCLASS()
// PROTOTYPE_API는 실제 프로젝트 API 매크로로 변경
class PROTOTYPE_API APrototypeGameMode : public AGameModeBase // 올바른 부모 클래스 상속 확인
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    bool isClearedBasic = false;
    UPROPERTY(EditAnywhere)
    bool isClearedFlame = false;
    UPROPERTY(EditAnywhere)
    bool isClearedWater = false;
    UPROPERTY(EditAnywhere)
    bool isClearedElec = false;

    UPROPERTY(EditAnywhere)
    bool isGotBasickey = false;
    UPROPERTY(EditAnywhere)
    bool isGotFlamekey = false;
    UPROPERTY(EditAnywhere)
    bool isGotWaterkey = false;
    UPROPERTY(EditAnywhere)
    bool isGotEleckey = false;


    void IsClearedMap(bool isCleared);

    void IsGotKey(bool isGotKey);

    // --- 물리 그랩 기능 관련 멤버 변수 추가 ---
  /** 현재 레벨에서 물리 그랩 기능이 활성화되었는지 여부입니다. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|PhysicsGrab")
    bool bPysGrabEnabledForCurrentLevel; // 이름 변경 및 명확화
    // --- 물리 그랩 기능 관련 함수 추가 ---
  /**
   * @brief 현재 레벨에서 물리 그랩 기능의 활성화 상태를 설정합니다.
   * @param bEnable true로 설정하면 현재 레벨에서 물리 그랩을 활성화합니다.
   */
    UFUNCTION(BlueprintCallable, Category = "Gameplay|PhysicsGrab")
    void SetPysGrabActiveForLevel(bool bEnable);

    /**
     * @brief 현재 레벨에서 물리 그랩 기능이 활성화되어 있는지 확인합니다.
     * @return 물리 그랩이 활성화되어 있으면 true를 반환합니다.
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay|PhysicsGrab")
    bool IsPysGrabActiveForLevel() const;

    APrototypeGameMode();

    // 레벨 시작 시 GameInstance에서 영구적 잠금 해제 상태를 가져와 적용하기 위해 BeginPlay 오버라이드
    virtual void BeginPlay() override;
private:
 
};