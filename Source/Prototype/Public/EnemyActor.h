// EnemyActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UBillboardComponent; // << ������ ������Ʈ ��� ���� (���� include�� cpp����)

UCLASS()
class PROTOTYPE_API AEnemyActor : public AActor // << YOURPROJECT_API�� PROTOTYPE_API�� �����ߴٰ� ����
{
    GENERATED_BODY()

public:
    AEnemyActor();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override; // << OnConstruction ����

public:
    virtual void Tick(float DeltaTime) override;

    // �������Ʈ Ŭ���� ����Ʈ�� �ν��Ͻ����� ������ �ִϸ��̼��� �����մϴ�.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimSequenceBase* MoveAnimation;

    // �������Ʈ Ŭ���� ����Ʈ�� �ν��Ͻ����� ���(����) �ִϸ��̼��� �����մϴ�.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimSequenceBase* IdleAnimation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USkeletalMeshComponent* CharacterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UNiagaraComponent* ElectroShockEffect;

    // �����Ϳ��� ������ �⺻ ���� ��� ����Ʈ �ּ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
        UNiagaraSystem* DefaultElectroShockFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float Speed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float RotationSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (MakeEditWidget = true)) // MakeEditWidget���� ��ġ ���� ����
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

#if WITH_EDITORONLY_DATA // ������ ���� ���������� ���
private:
    // ��Ʈ�� ���� �ð�ȭ�� ���� ������ ������Ʈ��
    UPROPERTY() // VisibleAnywhere ������ ������ �ʿ�� ����. ���� ������.
        UBillboardComponent* PatrolPointVisualizerA;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerB;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerC;
    UPROPERTY()
        UBillboardComponent* PatrolPointVisualizerD;
#endif



private:
    // ���� �ִϸ��̼� ���� ������ (���� ����������, �� ������ PlayAnimation ȣ�� ����)
    UPROPERTY(Transient) // ������ �ʿ� ����, �Ͻ����� ����
        UAnimSequenceBase* CurrentPlayingAnimation;

    bool bWasMovingLastFrame; // ���� �������� �̵� ����

    // �ִϸ��̼� ������Ʈ �Լ�
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