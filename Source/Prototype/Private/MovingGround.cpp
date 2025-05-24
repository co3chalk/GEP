// MovingGround.cpp (수정본)
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

    // ... (머티리얼 적용 로직은 동일) ...
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

    bPowered = false; // 시작은 항상 Unpowered
    SetActorLocation(StartPosition); // 시작 위치 고정

    // 시작 상태 설정
    if (bMoveToToggle)
    {
        // 움직이는 땅: 항상 시작 위치에서 보이도록 설정
        SetActorHiddenInGame(false);
        SetActorEnableCollision(true);
        SetActorTickEnabled(true);
    }
    else
    {
        // 사라지는 땅: bInvertDisappear 옵션에 따라 시작 상태 결정
        // bInvertDisappear = false (기본: P=V) -> Unpowered = Hidden -> 시작 시 숨김
        // bInvertDisappear = true (반전: P=H) -> Unpowered = Visible -> 시작 시 보임
        bool bStartHidden = !bInvertDisappear;
        SetActorHiddenInGame(bStartHidden);
        SetActorEnableCollision(!bStartHidden);
        SetActorTickEnabled(false);
    }
}

// Tick 함수는 이전과 동일
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

// SetPowered 함수는 이전과 동일
void AMovingGround::SetPowered(bool bOn)
{
    if (bPowered != bOn)
    {
        bPowered = bOn;
        UpdateState(bPowered);
    }
}

// UpdateState 함수 수정
void AMovingGround::UpdateState(bool bPowerStatus)
{
    if (bMoveToToggle)
    {
        SetActorTickEnabled(true);
    }
    else
    {
        // 사라지는 땅: bInvertDisappear 옵션에 따라 숨김/보임 결정
        // bInvertDisappear = false (P=V) -> Powered = Visible -> Hide = !bPowerStatus
        // bInvertDisappear = true  (P=H) -> Powered = Hidden  -> Hide = bPowerStatus
        bool bShouldBeHidden = bInvertDisappear ? bPowerStatus : !bPowerStatus;

        SetActorHiddenInGame(bShouldBeHidden);
        SetActorEnableCollision(!bShouldBeHidden);
        SetActorTickEnabled(false);
    }
}