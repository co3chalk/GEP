// ElecSwitch.h (������)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElecSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent; // <-- ����ƽ �޽� ������Ʈ ���� ����

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
    // ����ġ�� ������ ����� ����ƽ �޽� ������Ʈ �߰�
    UPROPERTY(VisibleAnywhere, Category = "Switch")
    UStaticMeshComponent* SwitchMesh = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Switch")
    UBoxComponent* Trigger = nullptr;

    UPROPERTY(EditAnywhere, Category = "Switch")
    TArray<AActor*> PoweredTargets;

    int32 OverlapCount = 0;

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