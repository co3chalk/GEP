#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElecSwitch.generated.h"

class UBoxComponent;

UCLASS()
class PROTOTYPE_API AElecSwitch : public AActor
{
    GENERATED_BODY()

public:
    AElecSwitch();
    UFUNCTION(BlueprintPure) bool IsPressed() const { return OverlapCount > 0; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Switch")
    UBoxComponent* Trigger = nullptr;

    UPROPERTY(EditAnywhere, Category = "Switch")
    TArray<AActor*> PoweredTargets;

    int32 OverlapCount = 0;

    /* 이름 추가! */
    UFUNCTION()
    void OnBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32              OtherBodyIndex,
        bool               bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEnd(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32              OtherBodyIndex);

    void SetTargetsPowered(bool bOn);
};
