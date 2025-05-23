#include "ElecBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"
#include "Battery.h"

AElecBullet::AElecBullet()
{
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(12.f);
    Collision->OnComponentHit.AddDynamic(this, &AElecBullet::OnHit);

    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionObjectType(ECC_GameTraceChannel1);   //"Bullet" 채널로 설정
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); //Enemy 전용

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
    ElecEffect->SetAutoActivate(true); // 자동 실행


    InitialLifeSpan = 8.f;
}

void AElecBullet::OnHit(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, FVector, const FHitResult& Hit)
{
    if (AEnemyActor* Enemy = Cast<AEnemyActor>(Other))
        Enemy->Freeze(2.5f);

    else if (ABattery* Battery = Cast<ABattery>(Other))
        Battery->Charge();

    if (ImpactFX)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, Hit.ImpactPoint);

    Destroy();
}
