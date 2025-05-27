// Fill out your copyright notice in the Description page of Project Settings.


#include "Sponge.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ASponge::ASponge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = staticMesh;
	staticMesh->SetSimulatePhysics(true);

	RopeConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RopeConstraint"));
	RopeConstraint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASponge::BeginPlay()
{
	Super::BeginPlay();
	staticMesh->OnComponentHit.AddDynamic(this, &ASponge::OnBulletHit);
}

// Called every frame
void ASponge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASponge::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("WaterBullet"))) // 태그로 구분
    {
        CurrentWeight += 1.0f;
        UE_LOG(LogTemp, Warning, TEXT("Weight: %f"), CurrentWeight);

        if (CurrentWeight >= WeightThreshold)
        {
            RopeConstraint->BreakConstraint();
        }

        OtherActor->Destroy(); // 물총알 제거
    }
}