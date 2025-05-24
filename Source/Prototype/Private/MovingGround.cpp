// MovingGround.cpp (������)
#include "MovingGround.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"

AMovingGround::AMovingGround()
{
    PrimaryActorTick.bCanEverTick = true;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AMovingGround::BeginPlay()
{
    Super::BeginPlay();

    // ... (��Ƽ���� ���� ������ ����) ...
    if (bMoveToToggle && MovingMaterial) { Mesh->SetMaterial(0, MovingMaterial); }
    else if (!bMoveToToggle && DisappearingMaterial) { Mesh->SetMaterial(0, DisappearingMaterial); }


    StartPosition = GetActorLocation();
    FVector OffsetDirection;
    switch (MovementAxis)
    {
    case EMovementAxis::X_Axis: OffsetDirection = FVector(1.f, 0.f, 0.f); break;
    case EMovementAxis::Y_Axis: OffsetDirection = FVector(0.f, 1.f, 0.f); break;
    case EMovementAxis::Z_Axis:
    default:                  OffsetDirection = FVector(0.f, 0.f, 1.f); break;
    }
    EndPosition = StartPosition + OffsetDirection * MoveDistance;

    bPowered = false; // ������ �׻� Unpowered
    SetActorLocation(StartPosition); // ���� ��ġ ����

    // ���� ���� ����
    if (bMoveToToggle)
    {
        // �����̴� ��: �׻� ���� ��ġ���� ���̵��� ����
        SetActorHiddenInGame(false);
        SetActorEnableCollision(true);
        SetActorTickEnabled(true);
    }
    else
    {
        // ������� ��: bInvertDisappear �ɼǿ� ���� ���� ���� ����
        // bInvertDisappear = false (�⺻: P=V) -> Unpowered = Hidden -> ���� �� ����
        // bInvertDisappear = true (����: P=H) -> Unpowered = Visible -> ���� �� ����
        bool bStartHidden = !bInvertDisappear;
        SetActorHiddenInGame(bStartHidden);
        SetActorEnableCollision(!bStartHidden);
        SetActorTickEnabled(false);
    }
}

// Tick �Լ��� ������ ����
void AMovingGround::Tick(float Dt)
{
    Super::Tick(Dt);
    if (bMoveToToggle)
    {
        const FVector Target = bPowered ? EndPosition : StartPosition;
        FVector CurrentLocation = GetActorLocation();
        if (FVector::DistSquared(CurrentLocation, Target) < 1.0f)
        {
            SetActorLocation(Target);
        }
        else
        {
            SetActorLocation(FMath::VInterpTo(CurrentLocation, Target, Dt, MoveInterpSpeed));
        }
    }
}

// SetPowered �Լ��� ������ ����
void AMovingGround::SetPowered(bool bOn)
{
    if (bPowered != bOn)
    {
        bPowered = bOn;
        UpdateState(bPowered);
    }
}

// UpdateState �Լ� ����
void AMovingGround::UpdateState(bool bPowerStatus)
{
    if (bMoveToToggle)
    {
        SetActorTickEnabled(true);
    }
    else
    {
        // ������� ��: bInvertDisappear �ɼǿ� ���� ����/���� ����
        // bInvertDisappear = false (P=V) -> Powered = Visible -> Hide = !bPowerStatus
        // bInvertDisappear = true  (P=H) -> Powered = Hidden  -> Hide = bPowerStatus
        bool bShouldBeHidden = bInvertDisappear ? bPowerStatus : !bPowerStatus;

        SetActorHiddenInGame(bShouldBeHidden);
        SetActorEnableCollision(!bShouldBeHidden);
        SetActorTickEnabled(false);
    }
}