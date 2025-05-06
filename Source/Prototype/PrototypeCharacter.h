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

	// 실린더 시각화 메쉬 갱신 함수
	void UpdateGrabVisualMesh();
	void RotateCharacterToMouse();

private:
	// 카메라
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	// 그랩 관련
	UPROPERTY(VisibleAnywhere)
	class UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(VisibleAnywhere)
	class UPhysicsConstraintComponent* RotationConstraint;

	UPROPERTY()
	UPrimitiveComponent* GrabbedComponent;

	UPROPERTY()
	class UStaticMeshComponent* GrabVisualMesh;

	// 라인트레이스용
	FVector LineStart;
	FVector LineEnd;
	FHitResult CachedHitResult;

	// 거리
	float GrabbedObjectDistance;
	float GrabMaxDistance;
	float GrabMinDistance;
	float ScrollDistanceSpeed;

	// 상태 플래그
	bool bIsLineTraceHit;
	bool bIsGrabbingNonPhysics;
	bool bIsRightMouseButtonDown;

	// 비물리 그랩
	FVector NonPhysicsHitLocation;
	float NonPhysicsGrabDistance;
	float NonPhysicsDistanceReduceSpeed;
	

	// 줄 반경 제한
	bool bIsWithinRopeRadius;

	// 아무것도 못 잡았을 때 시각 효과
	FVector MissedGrabTarget;
	bool bShowMissedGrabVisual = false;
	float MissedGrabTimer = 0.0f;
	float MissedGrabDuration = 0.3f;

	bool bNonPhysicsCollisionOccurred; // 비물리 줄 당기기 중 충돌 발생 여부


	// Swing(그네) 관련 추가 변수
	UPROPERTY(EditAnywhere, Category = "Swinging")
	float CorrectionStrength = 500.0f;  // 로프 장력 세기

	UPROPERTY(EditAnywhere, Category = "Swinging")
	float SwingForce = 1200.0f; // 입력으로 가해지는 추가 흔들기 힘
	// 헤더 (.h)
	TArray<FVector> SafeLocationHistory;
	int32 SafeLocationFrameDelay = 5; // 몇 프레임 전 위치로 되돌릴지
	int32 SafeLocationMaxFrames = 10; // 최대 저장 프레임 수
	bool bIsOverlapping = false;


protected:
	bool bPullBlockedDueToCollision = false;

};
