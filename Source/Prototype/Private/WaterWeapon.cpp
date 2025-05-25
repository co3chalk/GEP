// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterWeapon.h"

// Sets default values for this component's properties
UWaterWeapon::UWaterWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWaterWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWaterWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UWaterWeapon::StartFire()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
	{
		SpawnWater(); // 처음 한 발
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UWaterWeapon::SpawnWater, FireInterval, true);
	}
}

void UWaterWeapon::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UWaterWeapon::SpawnWater()
{
	if (!WaterProjectileClass) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector SpawnLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
	FRotator SpawnRot = Owner->GetActorRotation();

	GetWorld()->SpawnActor<AActor>(WaterProjectileClass, SpawnLoc, SpawnRot);
}