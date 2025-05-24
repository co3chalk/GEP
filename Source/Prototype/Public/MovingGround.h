// MovingGround.h (수정본)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingGround.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EMovementAxis : uint8
{
    X_Axis UMETA(DisplayName = "X Axis"),
    Y_Axis UMETA(DisplayName = "Y Axis"),
    Z_Axis UMETA(DisplayName = "Z Axis")
};

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
    UPROPERTY(VisibleAnywhere, Category = "MovingGround")
    UStaticMeshComponent* Mesh = nullptr;

    /* 옵션 */
    // true: 이동 / false: 사라짐/나타남
    UPROPERTY(EditAnywhere, Category = "MovingGround|Options", meta = (DisplayName = "Move To Toggle Visibility"))
    bool  bMoveToToggle = true;

    // '사라지는 땅'일 때, 동작을 반전시킬지 여부 (체크 시: 전원 공급 시 사라짐)
    UPROPERTY(EditAnywhere, Category = "MovingGround|Options", meta = (DisplayName = "Invert Disappear (Powered = Hidden)", EditCondition = "!bMoveToToggle"))
    bool bInvertDisappear = false;

    UPROPERTY(EditAnywhere, Category = "MovingGround|Movement")
    EMovementAxis MovementAxis = EMovementAxis::Z_Axis;

    UPROPERTY(EditAnywhere, Category = "MovingGround|Movement")
    float MoveDistance = 300.f;

    UPROPERTY(EditAnywhere, Category = "MovingGround|Movement", meta = (ClampMin = "0.1"))
    float MoveInterpSpeed = 3.f;

    UPROPERTY(EditAnywhere, Category = "MovingGround|Appearance")
    UMaterialInterface* MovingMaterial = nullptr;

    UPROPERTY(EditAnywhere, Category = "MovingGround|Appearance")
    UMaterialInterface* DisappearingMaterial = nullptr;

    bool    bPowered = false;
    FVector StartPosition;
    FVector EndPosition;

    void UpdateState(bool bPowerStatus);
};