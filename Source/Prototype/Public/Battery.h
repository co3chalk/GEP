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

    /* ElecBullet�� �ε��� �� ȣ�� */
    void Charge();

    /* �ܺ� ��ġ�� ���� ���θ� ��� �� */
    UFUNCTION(BlueprintPure) bool IsPowered() const { return ChargeCount > 0; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    /* ---------- �Ŀ� ���� ---------- */
    int32  ChargeCount = 0;          // 0~3
    float  RemainingPowerTime = 0.f;        // �ǽð� ����
    const  float ChargeUnit = 3.f;        // �� ���� 3��

    /* ---------- �ð�ȭ ---------- */
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BodyMesh;
    UPROPERTY(VisibleAnywhere) TArray<UStaticMeshComponent*> Indicators; // 0~2

    void UpdateIndicators();
};
