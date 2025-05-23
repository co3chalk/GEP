// ElecBullet.cpp
#include "ElecBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"
#include "Battery.h"
#include "NiagaraFunctionLibrary.h"

AElecBullet::AElecBullet()
{
    // ... (이전과 동일)
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(12.f);
    Collision->OnComponentHit.AddDynamic(this, &AElecBullet::OnHit);

    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_GameTraceChannel1);   // "Bullet" 채널

    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // Enemy 전용

    // Battery 오브젝트 타입에 대한 응답 추가:
    // 만약 Battery가 ECC_PhysicsBody 타입으로 설정되었다면:
    Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = 2000.f;
    Movement->MaxSpeed = 3000.f;
    Movement->bRotationFollowsVelocity = true;
    Movement->ProjectileGravityScale = 0.0f;

    ElecEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricEffect"));
    ElecEffect->SetupAttachment(RootComponent);
    ElecEffect->SetAutoActivate(true);

    InitialLifeSpan = 8.f;
}

void AElecBullet::OnHit(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, FVector, const FHitResult& Hit)
{
    // 충돌 시 이펙트 생성 (선택 사항)
    if (ImpactEffect)
    {
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::None, true);

        // 디버깅을 위한 로그 추가
        if (NiagaraComp == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("AElecBullet::OnHit - Failed to spawn NiagaraComponent for ImpactEffect. ImpactEffect asset might be invalid or scene component setup is incorrect."));
        }
        else
        {
            NiagaraComp->Activate(true);
            UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Successfully activated NiagaraComponent."));
        }
    }
    // else if (ImpactFX) 블록은 UNiagaraSystem* ImpactEffect가 존재한다면 실행되지 않습니다.
    // 기존 UParticleSystem* ImpactFX와 UNiagaraSystem* ImpactEffect 중 무엇을 우선할지 고려해야 합니다.
    // 현재 코드에서는 ImpactEffect(Niagara)가 우선됩니다.
    else if (ImpactFX) // 기존 파티클 시스템도 있다면
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true);
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Spawned ParticleSystem for ImpactFX."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - No ImpactEffect (Niagara) or ImpactFX (Particle) assigned."));
    }

    if (AEnemyActor* Enemy = Cast<AEnemyActor>(Other))
    {
        Enemy->Freeze(0.5f);
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Enemy: %s"), *Other->GetName());
    }
    else if (ABattery* Battery = Cast<ABattery>(Other))
    {
        Battery->Charge();
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Battery: %s"), *Other->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - Hit unknown actor: %s"), *Other->GetName());
    }

    // 총알 파괴 (충돌 후)
    Destroy();
}