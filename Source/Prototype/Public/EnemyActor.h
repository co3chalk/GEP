#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

// ���� ����
class UCapsuleComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class PROTOTYPE_API AEnemyActor : public AActor
{
    GENERATED_BODY()

public:
    AEnemyActor();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "EnemyAbilities")
    void Freeze(float Seconds);

    UFUNCTION(BlueprintCallable, Category = "EnemyAbilities")
    void Slowdown(float Seconds);

protected:
    virtual void BeginPlay() override;

private:
    // ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* CharacterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
    UNiagaraComponent* ElectroShockEffect;

    // ��Ʈ�� ���� ������Ƽ
    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetA = FVector(-400.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetB = FVector(400.f, 0.f, 0.f);

    // �� ���� �߰� ��Ʈ�� ������
    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetC = FVector(400.f, 400.f, 0.f); // ���� ��

    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetD = FVector(-400.f, 400.f, 0.f); // ���� ��

    // ���� ���� ��ǥ ��Ʈ�� ������
    TArray<FVector> PatrolPoints;
    // ���� ��ǥ ��Ʈ�� ���� �ε���
    int32 CurrentPatrolPointIndex;


    // ���� ���� ������Ƽ
    bool bIsFrozen = false;
    FTimerHandle FreezeTimerHandle;
    FTimerHandle SlowdownTimerHandle;

    // �̵� �� ȸ�� �ӵ� ���� ������Ƽ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float Speed = 150.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float CurrentSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float RotationSpeed = 10.0f;

    // ���� �Լ�
    void Unfreeze();
    void RestoreSpeed();
    void ApplyElectroShockEffect();

    // Niagara ����Ʈ �ּ� (�����Ϳ��� ����)
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    UNiagaraSystem* DefaultElectroShockFX;
};