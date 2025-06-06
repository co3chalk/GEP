#include "InputManager.h"
#include "Shooter.h"
#include "PrototypeCharacter.h"
#include "WaterWeapon.h"
#include "ElectricWeapon.h"
#include "FlameWeapon.h"


UInputManager::UInputManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInputManager::BeginPlay()
{
	Super::BeginPlay();

	Shooter = GetOwner()->FindComponentByClass<UShooter>();
	Flame = GetOwner()->FindComponentByClass<UFlameWeapon>();
	Water = GetOwner()->FindComponentByClass<UWaterWeapon>();
	Elec = GetOwner()->FindComponentByClass<UElectricWeapon>();
	OwnerChar = Cast<APrototypeCharacter>(GetOwner());
}

UActorComponent* UInputManager::GetActiveComponent() const
{
	//return bUseElectric ? Cast<UActorComponent>(Elec) : Cast<UActorComponent>(Shooter);
	if (bUseElectric)
		return Elec;
	if (bUseWater)
		return Water;
	if (bUseFlame)
		return Flame;
	return Shooter;
}

void UInputManager::HandleSwapWeapon()
{
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (MapName == TEXT("ElecMap"))
	{
		bUseElectric = !bUseElectric;
		bUseWater = false;
		bUseFlame = false;
	}
	else if (MapName == TEXT("WaterMap"))
	{
		bUseWater = !bUseWater;
		bUseElectric = false;
		bUseFlame = false;
	}
	else if (MapName == TEXT("FlameMap"))
	{
		bUseFlame = !bUseFlame;
		bUseElectric = false;
		bUseWater = false;
	}

	if (OwnerChar) //
	{
		OwnerChar->NotifyWeaponChanged();
	}
}

void UInputManager::HandleGrab()
{
	if (!OwnerChar) return;

	// 한 번 클릭 → 회전 예약
	UE_LOG(LogTemp, Warning, TEXT("true"));

	OwnerChar->bShouldRotateToMouse = true;
	OwnerChar->bWaitingForPostRotationAction = true;

	// 회전 완료 후 실행할 동작 예약
	OwnerChar->PostRotationAction = [this]()
		{
			if (bUseElectric)
			{
				if (Elec) Elec->Fire();
			}
			else if (bUseWater)
			{
				UE_LOG(LogTemp, Warning, TEXT("InputManager calling Water->SpawnWater()"));
				if (Water) Water->StartFire();
			}
			else if (bUseFlame)
			{
				UE_LOG(LogTemp, Warning, TEXT("InputManager calling Flame->Fire()"));
				if (Flame) Flame->StartFire();
			}
			else
			{
				if (Shooter) Shooter->Grab();
			}
		};
}


void UInputManager::HandleRelease()
{
	if (OwnerChar)
		OwnerChar->bShouldRotateToMouse = false;
	UE_LOG(LogTemp, Warning, TEXT("false"));

	if (!bUseElectric && Shooter)
		Shooter->Release();

	if (bUseWater)
	{
		if (Water) Water->StopFire();
	}
	if (bUseFlame)
	{
		if (Flame) Flame->StopFire();
	}
}


void UInputManager::HandleScrollUp()
{
	if (auto* Comp = GetActiveComponent())
	{
		if (auto* ShooterComp = Cast<UShooter>(Comp))
			ShooterComp->ScrollUp();
	}
}

void UInputManager::HandleScrollDown()
{
	if (auto* Comp = GetActiveComponent())
	{
		if (auto* ShooterComp = Cast<UShooter>(Comp))
			ShooterComp->ScrollDown();
	}
}

void UInputManager::HandleRightMouseDown()
{
	if (OwnerChar)
		OwnerChar->bShouldRotateToMouse = true;

	if (auto* Comp = GetActiveComponent())
	{
		if (auto* ShooterComp = Cast<UShooter>(Comp))
			ShooterComp->RightMouseDown();
	}
}


void UInputManager::HandleRightMouseUp()
{
	if (OwnerChar)
		OwnerChar->bShouldRotateToMouse = false;

	if (auto* Comp = GetActiveComponent())
	{
		if (auto* ShooterComp = Cast<UShooter>(Comp))
			ShooterComp->RightMouseUp();
	}
}
