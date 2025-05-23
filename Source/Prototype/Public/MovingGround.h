// MovingGround.h (수정본)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingGround.generated.h"

UCLASS()
class PROTOTYPE_API AMovingGround : public AActor
{
    GENERATED_BODY()

public:
    AMovingGround();

    void SetPowered(bool bOn);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh = nullptr;

    /* 옵션 */
    UPROPERTY(EditAnywhere) bool  bDisappearByMove = true;
    UPROPERTY(EditAnywhere) float MoveOffsetZ = -300.f;
    UPROPERTY(EditAnywhere) float MoveSpeed = 250.f;

    bool    bPowered = false;
    FVector ActivePos;
    FVector HiddenPos;
};
