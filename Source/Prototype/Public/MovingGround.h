// MovingGround.h (������)
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

    /* �ɼ� */
    // true: �̵� / false: �����/��Ÿ��
    UPROPERTY(EditAnywhere, Category = "MovingGround|Options", meta = (DisplayName = "Move To Toggle Visibility"))
    bool  bMoveToToggle = true;

    // '������� ��'�� ��, ������ ������ų�� ���� (üũ ��: ���� ���� �� �����)
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