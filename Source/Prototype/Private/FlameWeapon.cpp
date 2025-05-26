// Fill out your copyright notice in the Description page of Project Settings.


#include "FlameWeapon.h"

// Sets default values for this component's properties
UFlameWeapon::UFlameWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFlameWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFlameWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFlameWeapon::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("FlameWeapon Fire called"));
}
