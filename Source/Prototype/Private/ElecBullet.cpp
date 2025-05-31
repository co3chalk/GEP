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
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(12.f);
    // OnComponentHit ���ε� �� UFUNCTION() ��ũ�ΰ� ���� �Լ��� �ñ״�ó�� ��ġ�ؾ� �մϴ�.
    // ��� ������ OnHit �Լ� ���� UFUNCTION()�� �ְ�, �Ķ���Ͱ� �ùٸ��� Ȯ���ϼ���.
    Collision->OnComponentHit.AddDynamic(this, &AElecBullet::OnHit);

    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_GameTraceChannel1);    // "Bullet" ä��

    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // �̹� Pawn�� �����ϵ��� �����Ǿ� ����
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // Enemy ����
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
    // �浹 �� ����Ʈ ���� (���� ����)
    if (ImpactEffect)
    {
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::None, true);

        // ������� ���� �α� �߰�
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
  
    else if (ImpactFX)
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
        Enemy->Freeze(3.0f);
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

    // �Ѿ� �ı� (�浹 ��)
    Destroy();
}