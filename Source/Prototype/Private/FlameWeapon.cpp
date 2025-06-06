// Fill out your copyright notice in the Description page of Project Settings.


#include "FlameWeapon.h"
#include "PrototypeCharacter.h" // APrototypeCharacter ��� �߰�
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


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
	OwnerChar = Cast<APrototypeCharacter>(GetOwner());
	// ...
	
}


// Called every frame
void UFlameWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UFlameWeapon::StartFire()
{
	if (OwnerChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartFire: Showing flame cylinder"));
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerChar->GetActorLocation());
		OwnerChar->SetFlameVisible(true);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StartFire: OwnerChar is null"));
	}
}

void UFlameWeapon::StopFire()
{
	if (OwnerChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("StopFire: Hiding flame cylinder"));
		OwnerChar->SetFlameVisible(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StopFire: OwnerChar is null"));
	}
}