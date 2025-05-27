// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
// Sets default values
AWaterBullet::AWaterBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    RootComponent = CollisionComp;
    CollisionComp->SetGenerateOverlapEvents(true);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel10); // ���Ѿ� ���� ä��


    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // �÷��̾� ����
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Ignore); // �ٸ� ���Ѿ� ����
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Overlap); // ���������� ��ħ

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(CollisionComp);
   

    // Projectile movement ����
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 800.f;
    ProjectileMovement->MaxSpeed = 800.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // �߷� ���� ����

    InitialLifeSpan = LifeSpan;
}

// Called when the game starts or when spawned
void AWaterBullet::BeginPlay()
{
	Super::BeginPlay();
    SetLifeSpan(LifeSpan); // �� ������ LifeSpan ���� �ݿ�

    CollisionComp->OnComponentHit.AddDynamic(this, &AWaterBullet::OnHit);
}

// Called every frame
void AWaterBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaterBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("���Ѿ��� %s�� �浹�߽��ϴ�."), *OtherActor->GetName());
}