// Battery.h
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
    void Charge();
    UFUNCTION(BlueprintPure) bool IsPowered() const { return ChargeCount > 0; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    /* ---------- 파워 로직 ---------- */
    int32  ChargeCount = 0;
    float  RemainingPowerTime = 0.f;
    const  float ChargeUnit = 3.f;

    /* ---------- 시각화 및 충돌 ---------- */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* BodyMesh; // <-- 이 컴포넌트가 시각화와 충돌을 모두 담당합니다.
    UPROPERTY(VisibleAnywhere) TArray<UStaticMeshComponent*> Indicators;

    void UpdateIndicators();
};