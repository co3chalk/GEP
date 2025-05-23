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
    /* ---------- �Ŀ� ���� ---------- */
    int32  ChargeCount = 0;
    float  RemainingPowerTime = 0.f;
    const  float ChargeUnit = 3.f;

    /* ---------- �ð�ȭ �� �浹 ---------- */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* BodyMesh; // <-- �� ������Ʈ�� �ð�ȭ�� �浹�� ��� ����մϴ�.
    UPROPERTY(VisibleAnywhere) TArray<UStaticMeshComponent*> Indicators;

    void UpdateIndicators();
};