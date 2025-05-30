#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h" // FTimerHandle�� ���� �߰�
#include <functional>
#include "PrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UShooter;
class UElectricWeapon;
class UWaterWeapon;
class UFlameWeapon;
class UInputManager;

// HP ���� �� ȣ��� ��������Ʈ ���� (int32 ���)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, int32, CurrentHP, int32, MaxHP);
// ���� ���� ���� �� ȣ��� ��������Ʈ ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvincibilityChangedDelegate, bool, bIsNowInvincible);

UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

	void StartFire(); // ���� �Լ� ���� ����

	/* --- ü�� (HP) ���� --- */
	UFUNCTION(BlueprintPure, Category = "Health")
		int32 GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
		int32 GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void TakeDamage(int32 DamageAmount = 1);

	UFUNCTION(BlueprintPure, Category = "Health")
		bool IsInvincible() const;

	UPROPERTY(BlueprintAssignable, Category = "Health")
		FOnHPChangedDelegate OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
		FOnInvincibilityChangedDelegate OnInvincibilityChanged;


protected:
	virtual void BeginPlay() override;

	/* --- ü�� (HP) ���� --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
		int32 MaxHP = 6; // �ִ� HP 6

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
		int32 CurrentHP; // ���� HP

		/* --- ���� ���� ���� �� �Լ� --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
		bool bIsInvincible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float InvincibilityDuration = 5.0f;

	FTimerHandle InvincibilityTimerHandle;

	void StartInvincibility();
	void EndInvincibility();

	// HP ����/���� ó�� �Լ�
	void HandleHPChange();
	void Die();


public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* �̵� & ���� (���� �ڵ� ����) */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/* ���콺 �ٶ󺸱� (���� �ڵ� ����) */
	void RotateCharacterToMouse();
	bool bShouldRotateToMouse = false;
	bool IsRotationLocked() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UInputManager* InputManager;
	bool bWaitingForPostRotationAction = false;
	FRotator RotationTarget;
	TFunction<void()> PostRotationAction;

	/* ������ ���� bool ���� (���� �ڵ� ����) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")
		bool bGetEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flame_Energy")
		bool bGetFlameEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Water_Energy")
		bool bGetWaterEnergy = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Elctric_Energy")
		bool bGetElectricEnergy = false;

	void SetGetEnergy(bool bValue);
	void SetGetFlameEnergy(bool bValue);
	void SetGetWaterEnergy(bool bValue);
	void SetGetElectricEnergy(bool bValue);

	/* --- �ұ�� �����Լ� (���� �ڵ� ����) --- */
	void SetFlameCylinderVisible(bool bVisible);


private:
	/* --- ī�޶� ������Ʈ (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere) USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere) USceneComponent* CameraPivot;
	UPROPERTY(VisibleAnywhere) UCameraComponent* FollowCamera;

	/* --- ȭ����� �Ǹ��� �� �ݶ��̴� (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* FlameCylinderMesh;
	UPROPERTY(VisibleAnywhere, Category = "Flame")
	UCapsuleComponent* FlameCollider;
	bool bFlameCylinderVisible = false;

	/* --- ���� ������Ʈ (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UShooter* Shooter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UElectricWeapon* ElectricWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UWaterWeapon* WaterWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UFlameWeapon* FlameWeapon;
};