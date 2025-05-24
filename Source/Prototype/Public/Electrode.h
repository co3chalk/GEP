// Electrode.h (수정본)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Electrode.generated.h"

class UBoxComponent;
class ABattery;
class UStaticMeshComponent; // <-- 스태틱 메시 컴포넌트 전방 선언

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
    /* ---------- 컴포넌트 ---------- */
    // 전극의 외형을 담당할 스태틱 메시 컴포넌트 추가
    UPROPERTY(VisibleAnywhere, Category = "Electrode")
    UStaticMeshComponent* ElectrodeMesh = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Electrode")
    UBoxComponent* Detector = nullptr;

    /* ---------- 전원 상태 ---------- */
    TWeakObjectPtr<ABattery> ConnectedBattery;
    bool bWasPoweredLastFrame = false;

    UPROPERTY(EditAnywhere, Category = "Electrode")
    TArray<AActor*> PoweredTargets;

    /* ---------- 충돌 콜백 (파라미터 *모두 이름 기입*) ---------- */
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