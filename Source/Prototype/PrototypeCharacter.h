#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UShooter;

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

	/* Shooter ���°� ȸ���� ������ Ȯ�ο� */
	bool IsRotationLocked() const;

private:
	/* --- ī�޶� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;

	/* --- Grab/Swing ��� ������Ʈ --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UShooter* Shooter;
};
