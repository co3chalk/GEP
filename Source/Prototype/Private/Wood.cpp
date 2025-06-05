// Fill out your copyright notice in the Description page of Project Settings.


#include "Wood.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWood::AWood()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AWood::OnFlameOverlap);

	UE_LOG(LogTemp, Warning, TEXT("wood ready"));

}

// Called when the game starts or when spawned
void AWood::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("wood called"));
}

// Called every frame
void AWood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWood::OnFlameOverlap(UPrimitiveComponent* OverlappedComp,	AActor* OtherActor,	UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp && OtherComp->ComponentHasTag("Flame"))
	{
		UE_LOG(LogTemp, Warning, TEXT("wood burning start"));
		GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &AWood::BurnUp, BurnDelay, false);
	}
}

void AWood::BurnUp()
{
	UGameplayStatics::PlaySoundAtLocation(this, BurnSound, GetActorLocation());
	Destroy();
}