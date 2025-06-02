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
    //staticMesh->SetSimulatePhysics(true);

    staticMesh->SetCollisionObjectType(ECC_GameTraceChannel11); // Sponge 채널로 설정
    staticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    staticMesh->SetGenerateOverlapEvents(true);

    staticMesh->SetCollisionResponseToAllChannels(ECR_Block);
    staticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap);
}

// Called when the game starts or when spawned
void ASponge::BeginPlay()
{
	Super::BeginPlay();
    LockMotion();  // 처음에 모션 잠금 (고정)
    staticMesh->OnComponentBeginOverlap.AddDynamic(this, &ASponge::OnBulletOverlap);
}

// Called every frame
void ASponge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASponge::LockMotion()
{
    FBodyInstance* BodyInst = staticMesh->GetBodyInstance();
    if (BodyInst)
    {
        BodyInst->bLockXTranslation = true;
        BodyInst->bLockYTranslation = true;
        BodyInst->bLockZTranslation = true;

        BodyInst->bLockXRotation = true;
        BodyInst->bLockYRotation = true;
        BodyInst->bLockZRotation = true;

    }
}

void ASponge::UnLockMotion()
{
    FBodyInstance* BodyInst = staticMesh->GetBodyInstance();
    if (BodyInst)
    {
        BodyInst->bLockXTranslation = false;
        BodyInst->bLockYTranslation = false;
        BodyInst->bLockZTranslation = false;

        BodyInst->bLockXRotation = false;
        BodyInst->bLockYRotation = false;
        BodyInst->bLockZRotation = false;

    }

    staticMesh->SetEnableGravity(true);
}

void ASponge::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("WaterBullet")))
    {
        CurrentWeight += 1.0f;
        UE_LOG(LogTemp, Warning, TEXT("[Overlap] Weight: %f"), CurrentWeight);

        if (CurrentWeight >= WeightThreshold)
        {
            UnLockMotion();
            staticMesh->SetSimulatePhysics(true);

            if (ChangedStaticMesh)
            {
                staticMesh->SetStaticMesh(ChangedStaticMesh);
                staticMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.2f));
            }
        }

        OtherActor->Destroy();
    }
}
