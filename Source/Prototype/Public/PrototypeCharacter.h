// PrototypeCharacter.h

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
// 무기 변경 시 호출될 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedDelegate, const FString&, NewWeaponName);
// 에너지 개수 변경 시 호출될 델리게이트 선언 (새로 추가)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyCountChangedDelegate, int32, NewEnergyCount);


UCLASS()
class PROTOTYPE_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

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

	// 현재 무기 이름을 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FString GetCurrentWeaponName() const;

	// 이 델리게이트에 PlayerUIWidget이 바인딩합니다.
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponChangedDelegate OnWeaponChanged;

	void NotifyWeaponChanged();

	/* --- 에너지 관련 --- */
	// 에너지 값 GET 함수들 (BlueprintPure로 UI에서 직접 현재 값을 가져갈 수도 있도록)
	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetBasicEnergy() const { return basicEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetFlameEnergy() const { return flameEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetWaterEnergy() const { return waterEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetElectricEnergy() const { return electricEnergy; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	int32 GetKey() const { return Key; }

	// 에너지 변경 델리게이트 (새로 추가)
	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnBasicEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnFlameEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnWaterEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnElectricEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FOnEnergyCountChangedDelegate OnKeyChanged;


	// 에너지 획득 함수 (기존 유지, 내부에서 델리게이트 호출하도록 .cpp에서 수정)
	void SetGetBasicEnergy(bool bValue);
	void SetGetFlameEnergy(bool bValue);
	void SetGetWaterEnergy(bool bValue);
	void SetGetElectricEnergy(bool bValue);
	void SetGetKey(bool bValue);


protected:
	virtual void BeginPlay() override;
	/** 물리 그랩 잠금 해제 아이템과의 오버랩을 처리하는 함수 */
	UFUNCTION()
	void HandlePysGrabUnlockItemOverlap( // 함수 이름은 자유롭게 지정 가능
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	/* --- 체력 (HP) 변수 --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int32 MaxHP = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int32 CurrentHP; // 현재 HP

	/* --- 무적 관련 변수 및 함수 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float InvincibilityDuration = 2.0f;// 무적 지속 시간 (초 단위)

	FTimerHandle InvincibilityTimerHandle;

	void StartInvincibility();
	void EndInvincibility();

	// HP 변경/죽음 처리 함수
	void HandleHPChange();
	void Die();

	// 현재 무기 이름을 저장할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString CurrentWeaponDisplayName;

	/* --- 에너지 관련 int 변수 (기존 코드 유지) --- */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Energy") // 
		int basicEnergy = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") // 
		int flameEnergy = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") // 
		int waterEnergy = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") //
		int electricEnergy = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") //
		int Key = 1;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (DisplayName = "Physics Trace Start Socket Name"))
	FName CharacterMuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	USceneComponent* AttachedBusterMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName NozzleSocketNameOnBusterMesh;

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

	/* --- 불기둥 제어함수 (기존 코드 유지) --- */
	void SetFlameVisible(bool bVisible);

	/*--- UI 제어 함수 ---*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHPUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateWeaponUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateBEnergyUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateFEnergyUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateWEnergyUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateEEnergyUI();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateKeyUI();// 구현은 블루프린트

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void Pause();// 구현은 블루프린트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void IsDied();// 구현은 블루프린트

	/* --- 카메라 컴포넌트 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere) USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere) USceneComponent* CameraPivot;
	UPROPERTY(VisibleAnywhere) UCameraComponent* FollowCamera;

	/* --- 화염방사 실린더 및 콜라이더 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* FlameCylinderMesh;
	UPROPERTY(VisibleAnywhere) class UParticleSystemComponent* FlameParticle;
	UPROPERTY(VisibleAnywhere, Category = "Flame")
	class UCapsuleComponent* FlameCollider; // UCapsuleComponent* 로 수정
	bool bFlameCylinderVisible = false;

	/* --- 무기 컴포넌트 (기존 코드 유지) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UShooter* Shooter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UElectricWeapon* ElectricWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UWaterWeapon* WaterWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true")) UFlameWeapon* FlameWeapon;
};