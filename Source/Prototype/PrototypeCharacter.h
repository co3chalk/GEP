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

	/* 이동 & 점프 */
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/* 마우스 바라보기 */
	void RotateCharacterToMouse();

	/* Shooter 상태가 회전을 막는지 확인용 */
	bool IsRotationLocked() const;

private:
	/* --- 카메라 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;

	/* --- Grab/Swing 담당 컴포넌트 --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UShooter* Shooter;
};
