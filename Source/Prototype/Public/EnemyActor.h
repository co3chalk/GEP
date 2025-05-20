#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "EnemyActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROTOTYPE_API AEnemyActor : public AActor
{
    GENERATED_BODY()

public:
    AEnemyActor();
    virtual void Tick(float DeltaTime) override;
    void Freeze(float Seconds);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    FVector PatrolOffsetA = FVector(-400, 0, 0);

    UPROPERTY(EditAnywhere, Category = "Patrol")
    FVector PatrolOffsetB = FVector(400, 0, 0);

    FVector PatrolPointA;
    FVector PatrolPointB;

    bool bGoingToB = true;
    bool bIsFrozen = false;

    // 누락되어 있던 타이머 핸들 선언
    FTimerHandle FreezeTimerHandle;

    // 감전 이펙트
    UPROPERTY(VisibleAnywhere, Category = "Effect")
    UNiagaraComponent* ElectroShockEffect;


    void Unfreeze();
    void ApplyElectroShockEffect();
};
