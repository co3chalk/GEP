#pragma once

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElecBullet.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class PROTOTYPE_API AElecBullet : public AActor
{
    GENERATED_BODY()

public:
    AElecBullet();

    // ���� ����Ʈ (�Ѿ� �̵� ��)
    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* ElecEffect;

    // �浹 �� ���� ����Ʈ
    UPROPERTY(EditDefaultsOnly)
    UNiagaraSystem* ImpactEffect;


protected:
    UFUNCTION() void OnHit(
        UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
        FVector NormalImpulse, const FHitResult& Hit);

private:
    UPROPERTY(VisibleDefaultsOnly) USphereComponent* Collision;
    UPROPERTY(VisibleAnywhere)     UStaticMeshComponent * Mesh;

    UPROPERTY(EditDefaultsOnly)    UParticleSystem* ImpactFX = nullptr;
    UPROPERTY(VisibleAnywhere) UProjectileMovementComponent * Movement;
};
