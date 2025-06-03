// PrototypeCharacter.h

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
// ���� ���� �� ȣ��� ��������Ʈ ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedDelegate, const FString&, NewWeaponName);
// ������ ���� ���� �� ȣ��� ��������Ʈ ���� (���� �߰�)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyCountChangedDelegate, int32, NewEnergyCount);


UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

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

	// ���� ���� �̸��� ��ȯ�ϴ� �Լ�
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FString GetCurrentWeaponName() const;

	// �� ��������Ʈ�� PlayerUIWidget�� ���ε��մϴ�.
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponChangedDelegate OnWeaponChanged;

	void NotifyWeaponChanged();

	/* --- ������ ���� --- */
	// ������ �� GET �Լ��� (BlueprintPure�� UI���� ���� ���� ���� ������ ���� �ֵ���)
	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetBasicEnergy() const { return basicEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetFlameEnergy() const { return flameEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetWaterEnergy() const { return waterEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetElectricEnergy() const { return electricEnergy; }

	// ������ ���� ��������Ʈ (���� �߰�)
	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnBasicEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnFlameEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnWaterEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnElectricEnergyChanged;

	// ������ ȹ�� �Լ� (���� ����, ���ο��� ��������Ʈ ȣ���ϵ��� .cpp���� ����)
	void SetGetBasicEnergy(bool bValue);
	void SetGetFlameEnergy(bool bValue);
	void SetGetWaterEnergy(bool bValue);
	void SetGetElectricEnergy(bool bValue);


protected:
	virtual void BeginPlay() override;

	/* --- ü�� (HP) ���� --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int32 MaxHP = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int32 CurrentHP; // ���� HP

	/* --- ���� ���� ���� �� �Լ� --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float InvincibilityDuration = 2.0f;// ���� ���� �ð� (�� ����)

	FTimerHandle InvincibilityTimerHandle;

	void StartInvincibility();
	void EndInvincibility();

	// HP ����/���� ó�� �Լ�
	void HandleHPChange();
	void Die();

	// ���� ���� �̸��� ������ ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString CurrentWeaponDisplayName;

	/* --- ������ ���� int ���� (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy") // ī�װ��� �ϰ��� ���� Basic_Energy -> Energy
		int basicEnergy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy") // Flame_Energy -> Energy
		int flameEnergy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy") // Water_Energy -> Energy
		int waterEnergy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy") // Elctric_Energy -> Energy (��Ÿ ����)
		int electricEnergy = 0;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (DisplayName = "Physics Trace Start Socket Name"))
	FName CharacterMuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	USceneComponent* AttachedBusterMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName NozzleSocketNameOnBusterMesh;

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

	/* --- �ұ�� �����Լ� (���� �ڵ� ����) --- */
	void SetFlameVisible(bool bVisible);

	/*--- UI ���� �Լ� ---*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateUI();// ������ �������Ʈ

private:
	/* --- ī�޶� ������Ʈ (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere) USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere) USceneComponent* CameraPivot;
	UPROPERTY(VisibleAnywhere) UCameraComponent* FollowCamera;

	/* --- ȭ����� �Ǹ��� �� �ݶ��̴� (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* FlameCylinderMesh;
	UPROPERTY(VisibleAnywhere) class UParticleSystemComponent* FlameParticle;
	UPROPERTY(VisibleAnywhere, Category = "Flame")
	class UCapsuleComponent* FlameCollider; // UCapsuleComponent* �� ����
	bool bFlameCylinderVisible = false;

	/* --- ���� ������Ʈ (���� �ڵ� ����) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UShooter* Shooter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UElectricWeapon* ElectricWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UWaterWeapon* WaterWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UFlameWeapon* FlameWeapon;
};