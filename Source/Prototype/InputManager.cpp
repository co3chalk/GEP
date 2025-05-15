#include "InputManager.h"
#include "Shooter.h"
#include "PrototypeCharacter.h"
//추가기능 헤더. 주석해제하고, 클래스 이름맞춰 작성하기!
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

	// 회전 완료 후 Grab 실행 예약
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
