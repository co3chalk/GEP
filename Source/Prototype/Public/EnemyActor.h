#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

    // ?? 상대 좌표로 블루프린트에서 설정 가능
    UPROPERTY(EditAnywhere, Category = "Patrol")
    FVector PatrolOffsetA = FVector(-400, 0, 0);

    UPROPERTY(EditAnywhere, Category = "Patrol")
    FVector PatrolOffsetB = FVector(400, 0, 0);

    FVector PatrolPointA;
    FVector PatrolPointB;

    bool bGoingToB = true;
    float FrozenTimer = 0.f;
};
