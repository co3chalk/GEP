#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <functional>
#include "PrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UShooter;
class UElectricWeapon;
class UWaterWeapon;
class UInputManager;

UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

	void StartFire();

protected:
	virtual void BeginPlay() override;
	

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* �̵� & ���� */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/* ���콺 �ٶ󺸱� */
	void RotateCharacterToMouse();

	// ���콺 ȸ���� ������� ���� (��/�� Ŭ�� �߿��� true)
	bool bShouldRotateToMouse = false;


	/* Shooter ���°� ȸ���� ������ Ȯ�ο� */
	bool IsRotationLocked() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInputManager* InputManager;

	bool bWaitingForPostRotationAction = false;
	FRotator RotationTarget;
	TFunction<void()> PostRotationAction;

	/* ������ ���� bool ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")	//�̰� ��� �ɰ��� ����
	bool bGetEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire_Energy")
	bool bGetFireEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Water_Energy")
	bool bGetWaterEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Elctric_Energy")
	bool bGetElectricEnergy = false;


	void SetGetEnergy(bool bValue);  
	void SetGetFireEnergy(bool bValue);
	void SetGetWaterEnergy(bool bValue);
	void SetGetElectricEnergy(bool bValue);

private:
	/* --- ī�޶� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* CameraPivot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;


	/* --- Grab/Swing ��� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UShooter* Shooter;
	/* --- ������� ��� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UElectricWeapon* ElectricWeapon;
	/* --- ������ ��� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWaterWeapon* WaterWeapon;
};
