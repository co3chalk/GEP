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
void AElecBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // --- ����: OtherActor ��ȿ�� �˻� ---
    if (!OtherActor)
    {
        UE_LOG(LogTemp, Error, TEXT("AElecBullet::OnHit - OtherActor is NULL. Bullet: %s, HitComponent (Bullet's): %s."), *GetNameSafe(this), *GetNameSafe(HitComponent));
        Destroy(); // �Ѿ� �ı�
        return;
    }

    // �Ѿ��� �ڱ� �ڽŰ� �浹�� ��� (���� ���� ��)
    if (OtherActor == this)
    {
        // �� ���� ���� �����մϴ�. �ʿ信 ���� �α׸� ���� �� �ֽ��ϴ�.
        // ProjectileMovementComponent�� bInitialVelocityInLocalSpace = false �̰ų�, ���� ��ġ �������� �����ϴ� ���� �����ϴ�.
        UE_LOG(LogTemp, Verbose, TEXT("AElecBullet::OnHit - Bullet hit itself. Ignoring. Bullet: %s"), *GetNameSafe(this));
        return; // �ڱ� �ڽŰ��� �浹�� Ư���� ó���� ���� �ʰ� ��ȯ (�ı� X)
    }

    if (OtherActor->IsPendingKill())
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - OtherActor '%s' (Class: %s) is pending kill. Ignoring hit. Bullet: %s"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()), *GetNameSafe(this));
        Destroy(); // �Ѿ� �ı�
        return;
    }
    // --- ����: OtherActor ��ȿ�� �˻� ---

    // ���� OtherActor�� ��ȿ�� (nullptr �ƴ�, IsPendingKill �ƴ�, �ڱ� �ڽ� �ƴ�)

    // �浹 �� ����Ʈ ����
    if (ImpactEffect) // UNiagaraSystem* Ÿ���� UPROPERTY ����
    {
        // GetActorLocation() ��� Hit.ImpactPoint��, FRotator::ZeroRotator ��� Hit.ImpactNormal.Rotation()�� ����ϸ� �� ��Ȯ�� ��ġ/���⿡ ����Ʈ�� �����˴ϴ�.
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactEffect,
            Hit.ImpactPoint, // �浹 ����
            Hit.ImpactNormal.Rotation(), // �浹 ǥ���� ���� �������� ȸ��
            FVector(1.f), // ������
            true,         // �ڵ� �Ҹ�
            true,         // �ڵ� Ȱ��ȭ
            ENCPoolMethod::None,
            true          // ���� �������� Ǯ�� (bAutoActivate�� true�̸� �� �Ķ���Ͱ� ���� Ȱ��ȭ�� ����)
        );

        if (NiagaraComp == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("AElecBullet::OnHit - Failed to spawn NiagaraComponent for ImpactEffect. ImpactEffect asset might be invalid. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
        }
        else
        {
            // SpawnSystemAtLocation�� ������ �Ķ���� (bAutoActivate)�� true�̸� NiagaraComp->Activate(true)�� ���� �ʿ� �����ϴ�.
            UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Spawned Niagara ImpactEffect. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
        }
    }
    else if (ImpactFX) // UParticleSystem* Ÿ���� UPROPERTY ����
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ImpactFX,
            Hit.ImpactPoint,
            Hit.ImpactNormal.Rotation(),
            FVector(1.f),
            true
        );
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Spawned ParticleSystem ImpactFX. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - No ImpactEffect (Niagara) or ImpactFX (Particle) assigned. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
    }

    // OtherActor Ÿ�Կ� ���� ó�� (���� OtherActor->GetName() ���� �����ϰ� ��� ����)
    if (AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor))
    {
        Enemy->Freeze(3.0f); // Freeze �Լ� ���ο����� Enemy ��ȿ�� �˻簡 �ʿ��� �� ����
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Enemy: %s (Class: %s)"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()));
    }
    else if (ABattery* Battery = Cast<ABattery>(OtherActor))
    {
        Battery->Charge(); // Charge �Լ� ���ο����� Battery ��ȿ�� �˻簡 �ʿ��� �� ����
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Battery: %s (Class: %s)"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()));
    }
    else
    {
        // ��Ÿ ���� (��: ��, �ٴ� �� WorldStatic, WorldDynamic ��ü) �� �浹�� ���
        // �ݸ��� ������ ���� �� ���� �������� ������ �� �ֽ��ϴ�.
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit other actor: %s (Class: %s), Hit Component: %s (Class: %s)"),
            *OtherActor->GetName(),
            *GetNameSafe(OtherActor->GetClass()),
            *GetNameSafe(OtherComp), // ������ � ������Ʈ�� �¾Ҵ���
            *GetNameSafe(OtherComp ? OtherComp->GetClass() : nullptr)
        );
    }

    // �Ѿ� �ı� (��� ��ȣ�ۿ� ��)
    Destroy();
}