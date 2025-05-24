// Electrode.h (������)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Electrode.generated.h"

class UBoxComponent;
class ABattery;
class UStaticMeshComponent; // <-- ����ƽ �޽� ������Ʈ ���� ����

UCLASS()
class PROTOTYPE_API AElectrode : public AActor
{
    GENERATED_BODY()

public:
    AElectrode();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    /* ---------- ������Ʈ ---------- */
    // ������ ������ ����� ����ƽ �޽� ������Ʈ �߰�
    UPROPERTY(VisibleAnywhere, Category = "Electrode")
    UStaticMeshComponent* ElectrodeMesh = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Electrode")
    UBoxComponent* Detector = nullptr;

    /* ---------- ���� ���� ---------- */
    TWeakObjectPtr<ABattery> ConnectedBattery;
    bool bWasPoweredLastFrame = false;

    UPROPERTY(EditAnywhere, Category = "Electrode")
    TArray<AActor*> PoweredTargets;

    /* ---------- �浹 �ݹ� (�Ķ���� *��� �̸� ����*) ---------- */
    UFUNCTION()
    void OnBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32                OtherBodyIndex,
        bool                 bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEnd(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32                OtherBodyIndex);

    void SetTargetsPowered(bool bOn);
};