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

	/* 이동 & 점프 */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/* 마우스 바라보기 */
	void RotateCharacterToMouse();

	// 마우스 회전을 허용할지 여부 (좌/우 클릭 중에만 true)
	bool bShouldRotateToMouse = false;


	/* Shooter 상태가 회전을 막는지 확인용 */
	bool IsRotationLocked() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInputManager* InputManager;

	bool bWaitingForPostRotationAction = false;
	FRotator RotationTarget;
	TFunction<void()> PostRotationAction;

	/* 에너지 관련 bool 변수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")	//이거 없어도 될거임 ㅇㅇ
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
	/* --- 카메라 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* CameraPivot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;


	/* --- Grab/Swing 담당 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UShooter* Shooter;
	/* --- 전기공격 담당 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UElectricWeapon* ElectricWeapon;
	/* --- 물공격 담당 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWaterWeapon* WaterWeapon;
};
