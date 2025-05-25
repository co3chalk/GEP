#include "InputManager.h"
#include "Shooter.h"
#include "PrototypeCharacter.h"
//�߰���� ���. �ּ������ϰ�, Ŭ���� �̸����� �ۼ��ϱ�!
//#include "FireComponent.h"
#include "WaterWeapon.h"
#include "ElectricWeapon.h"


UInputManager::UInputManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInputManager::BeginPlay()
{
	Super::BeginPlay();

	Shooter = GetOwner()->FindComponentByClass<UShooter>();
	//Fire = GetOwner()->FindComponentByClass<UFireComponent>();
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
	return Shooter;
}

void UInputManager::HandleSwapWeapon()
{
	//bUseElectric = !bUseElectric;
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (MapName == TEXT("ElecMap"))
	{
		bUseElectric = !bUseElectric;
		bUseWater = false;
	}
	else if (MapName == TEXT("WaterMap"))
	{
		bUseWater = !bUseWater;
		bUseElectric = false;
	}
}

void UInputManager::HandleGrab()
{
	if (!OwnerChar) return;

	// �� �� Ŭ�� �� ȸ�� ����
	OwnerChar->bShouldRotateToMouse = true;
	OwnerChar->bWaitingForPostRotationAction = true;

	// ȸ�� �Ϸ� �� ������ ���� ����
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

	if (!bUseElectric && Shooter)
		Shooter->Release();

	if (bUseWater)
	{
		if (Water) Water->StopFire();
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
