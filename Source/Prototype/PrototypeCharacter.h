// PrototypeCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PrototypeCharacter.generated.h"

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

	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	void Grab();
	void Release();
	void ScrollUpFunction();
	void ScrollDownFunction();
	void RightMouseDown();
	void RightMouseUp();

	// �Ǹ��� �ð�ȭ �޽� ���� �Լ�
	void UpdateGrabVisualMesh();
	void RotateCharacterToMouse();

private:
	// ī�޶�
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	// �׷� ����
	UPROPERTY(VisibleAnywhere)
	class UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(VisibleAnywhere)
	class UPhysicsConstraintComponent* RotationConstraint;

	UPROPERTY()
	UPrimitiveComponent* GrabbedComponent;

	UPROPERTY()
	class UStaticMeshComponent* GrabVisualMesh;

	// ����Ʈ���̽���
	FVector LineStart;
	FVector LineEnd;
	FHitResult CachedHitResult;

	// �Ÿ�
	float GrabbedObjectDistance;
	float GrabMaxDistance;
	float GrabMinDistance;
	float ScrollDistanceSpeed;

	// ���� �÷���
	bool bIsLineTraceHit;
	bool bIsGrabbingNonPhysics;
	bool bIsRightMouseButtonDown;

	// �񹰸� �׷�
	FVector NonPhysicsHitLocation;
	float NonPhysicsGrabDistance;
	float NonPhysicsDistanceReduceSpeed;
	

	// �� �ݰ� ����
	bool bIsWithinRopeRadius;

	// �ƹ��͵� �� ����� �� �ð� ȿ��
	FVector MissedGrabTarget;
	bool bShowMissedGrabVisual = false;
	float MissedGrabTimer = 0.0f;
	float MissedGrabDuration = 0.3f;

	bool bNonPhysicsCollisionOccurred; // �񹰸� �� ���� �� �浹 �߻� ����


	// Swing(�׳�) ���� �߰� ����
	UPROPERTY(EditAnywhere, Category = "Swinging")
	float CorrectionStrength = 500.0f;  // ���� ��� ����

	UPROPERTY(EditAnywhere, Category = "Swinging")
	float SwingForce = 1200.0f; // �Է����� �������� �߰� ���� ��
	// ��� (.h)
	TArray<FVector> SafeLocationHistory;
	int32 SafeLocationFrameDelay = 5; // �� ������ �� ��ġ�� �ǵ�����
	int32 SafeLocationMaxFrames = 10; // �ִ� ���� ������ ��
	bool bIsOverlapping = false;


protected:
	bool bPullBlockedDueToCollision = false;

};
