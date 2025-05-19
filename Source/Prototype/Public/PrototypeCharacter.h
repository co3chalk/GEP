#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <functional>
#include "PrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UShooter;
class UElectricWeapon;
class UInputManager;

UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

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
};
