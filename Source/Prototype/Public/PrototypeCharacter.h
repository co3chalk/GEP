#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h" // FTimerHandle을 위해 추가
#include <functional>
#include "PrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UShooter;
class UElectricWeapon;
class UWaterWeapon;
class UFlameWeapon;
class UInputManager;

// HP 변경 시 호출될 델리게이트 선언 (int32 사용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, int32, CurrentHP, int32, MaxHP);
// 무적 상태 변경 시 호출될 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvincibilityChangedDelegate, bool, bIsNowInvincible);

UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

	void StartFire(); // 기존 함수 선언 유지

	/* --- 체력 (HP) 관련 --- */
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

	/* --- 체력 (HP) 변수 --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
		int32 MaxHP = 6; // 최대 HP 6

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
		int32 CurrentHP; // 현재 HP

		/* --- 무적 관련 변수 및 함수 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
		bool bIsInvincible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float InvincibilityDuration = 5.0f;

	FTimerHandle InvincibilityTimerHandle;

	void StartInvincibility();
	void EndInvincibility();

	// HP 변경/죽음 처리 함수
	void HandleHPChange();
	void Die();


public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* 이동 & 점프 (기존 코드 유지) */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/* 마우스 바라보기 (기존 코드 유지) */
	void RotateCharacterToMouse();
	bool bShouldRotateToMouse = false;
	bool IsRotationLocked() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UInputManager* InputManager;
	bool bWaitingForPostRotationAction = false;
	FRotator RotationTarget;
	TFunction<void()> PostRotationAction;

	/* 에너지 관련 bool 변수 (기존 코드 유지) */
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

	/* --- 불기둥 제어함수 (기존 코드 유지) --- */
	void SetFlameCylinderVisible(bool bVisible);


private:
	/* --- 카메라 컴포넌트 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere) USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere) USceneComponent* CameraPivot;
	UPROPERTY(VisibleAnywhere) UCameraComponent* FollowCamera;

	/* --- 화염방사 실린더 및 콜라이더 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* FlameCylinderMesh;
	UPROPERTY(VisibleAnywhere, Category = "Flame")
	UCapsuleComponent* FlameCollider;
	bool bFlameCylinderVisible = false;

	/* --- 무기 컴포넌트 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UShooter* Shooter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UElectricWeapon* ElectricWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UWaterWeapon* WaterWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UFlameWeapon* FlameWeapon;
};