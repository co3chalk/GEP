// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"

// Sets default values
AWaterBullet::AWaterBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    RootComponent = CollisionComp;
    CollisionComp->SetGenerateOverlapEvents(true);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel10); // 물총알 전용 채널


    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 플레이어 무시
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Ignore); // 다른 물총알 무시
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Overlap); // 스펀지랑만 겹침
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // 적 겹침

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(CollisionComp);
   

    // Projectile movement 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 700.f;
    ProjectileMovement->MaxSpeed = 700.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 영향 제거

    InitialLifeSpan = LifeSpan;
}

// Called when the game starts or when spawned
void AWaterBullet::BeginPlay()
{
	Super::BeginPlay();
    SetLifeSpan(LifeSpan); // 이 시점에 LifeSpan 값을 반영

    CollisionComp->OnComponentHit.AddDynamic(this, &AWaterBullet::OnHit);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWaterBullet::OnOverlapEnemy);

}

// Called every frame
void AWaterBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (ProjectileMovement)
    {
        FVector Velocity = ProjectileMovement->Velocity;
        float Speed = Velocity.Size();

        // 최소 속도 제한
        float MinSpeed = 100.f;
        float Deceleration = 300.f; // 초당 줄어드는 속도량

        if (Speed > MinSpeed)
        {
            float NewSpeed = FMath::Max(Speed - Deceleration * DeltaTime, MinSpeed);
            ProjectileMovement->Velocity = Velocity.GetSafeNormal() * NewSpeed;
        }
        else if (!bIsDestroyTimerSet)
        {
            // 아직 타이머가 안 걸렸으면 0.2초 후 Destroy
            bIsDestroyTimerSet = true;
            GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AWaterBullet::DestroyBullet, 0.2f, false);
        }
    }
}
void AWaterBullet::DestroyBullet()
{
    Destroy();
}
void AWaterBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("물총알이 %s에 충돌했습니다."), *OtherActor->GetName());
}

void AWaterBullet::OnOverlapEnemy(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("물총알이 %s와 겹침 발생"), *OtherActor->GetName());

    if (OtherActor && OtherActor != this)
    {
        AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor);
        if (Enemy)
        {
            Enemy->Slowdown(1.f); // 1초 동안 느려짐
            Destroy();
        }
    }

   
}
