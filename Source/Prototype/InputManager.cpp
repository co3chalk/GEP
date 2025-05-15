#include "InputManager.h"
#include "Shooter.h"
#include "PrototypeCharacter.h"
//�߰���� ���. �ּ������ϰ�, Ŭ���� �̸����� �ۼ��ϱ�!
//#include "FireComponent.h"
//#include "WaterComponent.h"
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
	//Water = GetOwner()->FindComponentByClass<UWaterComponent>();
	Elec = GetOwner()->FindComponentByClass<UElectricWeapon>();
	OwnerChar = Cast<APrototypeCharacter>(GetOwner());
}

UActorComponent* UInputManager::GetActiveComponent() const
{
	return bUseElectric ? Cast<UActorComponent>(Elec) : Cast<UActorComponent>(Shooter);
}

void UInputManager::HandleSwapWeapon()
{
	bUseElectric = !bUseElectric;
}

void UInputManager::HandleGrab()
{
	if (!OwnerChar) return;

	// ȸ�� �Ϸ� �� Grab ���� ����
	OwnerChar->bShouldRotateToMouse = true;
	OwnerChar->bWaitingForPostRotationAction = true;

	OwnerChar->PostRotationAction = [this]()
		{
			if (bUseElectric)
			{
				if (Elec) Elec->Fire();
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
