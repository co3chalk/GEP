#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Battery.generated.h"

UCLASS()
class PROTOTYPE_API ABattery : public AActor
{
    GENERATED_BODY()

public:
    ABattery();

    /* ElecBullet이 부딪힐 때 호출 */
    void Charge();

    /* 외부 장치가 전원 여부를 물어볼 때 */
    UFUNCTION(BlueprintPure) bool IsPowered() const { return ChargeCount > 0; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    /* ---------- 파워 로직 ---------- */
    int32  ChargeCount = 0;          // 0~3
    float  RemainingPowerTime = 0.f;        // 실시간 감소
    const  float ChargeUnit = 3.f;        // 한 번에 3초

    /* ---------- 시각화 ---------- */
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BodyMesh;
    UPROPERTY(VisibleAnywhere) TArray<UStaticMeshComponent*> Indicators; // 0~2

    void UpdateIndicators();
};
