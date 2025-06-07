// EnemyActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UBillboardComponent; // << 빌보드 컴포넌트 헤더 선언 (실제 include는 cpp에서)

UCLASS()
class PROTOTYPE_API AEnemyActor : public AActor // << YOURPROJECT_API를 PROTOTYPE_API로 수정했다고 가정
{
    GENERATED_BODY()

public:
    AEnemyActor();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override; // << OnConstruction 선언

public:
    virtual void Tick(float DeltaTime) override;

    // 블루프린트 클래스 디폴트나 인스턴스에서 움직임 애니메이션을 설정합니다.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimSequenceBase* MoveAnimation;

    // 블루프린트 클래스 디폴트나 인스턴스에서 대기(멈춤) 애니메이션을 설정합니다.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimSequenceBase* IdleAnimation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USkeletalMeshComponent* CharacterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UNiagaraComponent* ElectroShockEffect;

    // 에디터에서 설정할 기본 전기 충격 이펙트 애셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
        UNiagaraSystem* DefaultElectroShockFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float Speed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float RotationSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (MakeEditWidget = true)) // MakeEditWidget으로 위치 조정 가능
        FVector PatrolOffsetA = FVector(500.f, 0.f, 0.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (MakeEditWidget = true))
        FVector PatrolOffsetB = FVector(500.f, 500.f, 0.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (MakeEditWidget = true))
        FVector PatrolOffsetC = FVector(0.f, 500.f, 0.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (MakeEditWidget = true))
        FVector PatrolOffsetD = FVector(0.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        float FreezeDurationOnHit = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        float SlowdownDurationOnWaterHit = 10.0f;

#if WITH_EDITORONLY_DATA // 에디터 전용 데이터임을 명시
private:
    // 패트롤 지점 시각화를 위한 빌보드 컴포넌트들
    UPROPERTY() // VisibleAnywhere 등으로 노출할 필요는 없음. 내부 관리용.
        UBillboardComponent* PatrolPointVisualizerA;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerB;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerC;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerD;
#endif



private:
    // 현재 애니메이션 상태 추적용 (선택 사항이지만, 매 프레임 PlayAnimation 호출 방지)
    UPROPERTY(Transient) // 저장할 필요 없고, 일시적인 상태
        UAnimSequenceBase* CurrentPlayingAnimation;

    bool bWasMovingLastFrame; // 이전 프레임의 이동 상태

    // 애니메이션 업데이트 함수
    void UpdateAnimation();
    TArray<FVector> PatrolPoints;
    int32 CurrentPatrolPointIndex;
    float CurrentSpeed;
    bool bIsFrozen = false;

    FTimerHandle FreezeTimerHandle;
    FTimerHandle SlowdownTimerHandle;

    void ApplyElectroShockEffect();

    UFUNCTION()
        void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    void Freeze(float Seconds);
    void Unfreeze();
    void Slowdown(float Seconds);
    void RestoreSpeed();
};