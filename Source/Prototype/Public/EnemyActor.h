#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

// 전방 선언
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
    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* CharacterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
    UNiagaraComponent* ElectroShockEffect;

    // 패트롤 관련 프로퍼티
    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetA = FVector(-400.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetB = FVector(400.f, 0.f, 0.f);

    // 두 개의 추가 패트롤 오프셋
    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetC = FVector(400.f, 400.f, 0.f); // 예시 값

    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (MakeEditWidget = "true"))
    FVector PatrolOffsetD = FVector(-400.f, 400.f, 0.f); // 예시 값

    // 실제 월드 좌표 패트롤 지점들
    TArray<FVector> PatrolPoints;
    // 현재 목표 패트롤 지점 인덱스
    int32 CurrentPatrolPointIndex;


    // 상태 관련 프로퍼티
    bool bIsFrozen = false;
    FTimerHandle FreezeTimerHandle;
    FTimerHandle SlowdownTimerHandle;

    // 이동 및 회전 속도 관련 프로퍼티
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float Speed = 150.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float CurrentSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float RotationSpeed = 10.0f;

    // 내부 함수
    void Unfreeze();
    void RestoreSpeed();
    void ApplyElectroShockEffect();

    // Niagara 이펙트 애셋 (에디터에서 설정)
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    UNiagaraSystem* DefaultElectroShockFX;
};