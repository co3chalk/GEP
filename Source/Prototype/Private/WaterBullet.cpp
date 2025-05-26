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
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel10); // ���Ѿ� ���� ä��

    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // �÷��̾� ����
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Ignore); // �ٸ� ���Ѿ� ����

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(CollisionComp);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Projectile movement ����
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1200.f;
    ProjectileMovement->MaxSpeed = 1200.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // �߷� ���� ����

    InitialLifeSpan = LifeSpan;
}

// Called when the game starts or when spawned
void AWaterBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

