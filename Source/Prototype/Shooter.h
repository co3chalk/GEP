#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter.generated.h"

class UPhysicsHandleComponent;
class UPhysicsConstraintComponent;
class UStaticMeshComponent;

/**
 * Grab / Rope-Swing / Scroll / 우클릭 당기기 전담 컴포넌트
 *  ? APrototypeCharacter 안에 있던 동일 로직을 이동
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UShooter : public UActorComponent
{
	GENERATED_BODY()

public:
	UShooter();

	/*------------- APrototypeCharacter 에서 호출할 래퍼 -------------*/
	void Grab();
	void Release();
	void ScrollUp();
	void ScrollDown();
	void RightMouseDown();
	void RightMouseUp();

	UFUNCTION(BlueprintPure)
	bool ShouldLockRotation() const { return GrabbedComponent || bIsGrabbingNonPhysics; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	/* ====== APrototypeCharacter 에 있던 Grab 관련 ‘멤버 변수’ 전부 이동 ====== */
	UPROPERTY() UPhysicsHandleComponent* PhysicsHandle;
	UPROPERTY() UPhysicsConstraintComponent* RotationConstraint;
	UPROPERTY() UStaticMeshComponent* GrabVisualMesh;
	UPROPERTY() UPrimitiveComponent* GrabbedComponent = nullptr;

	// 카메라는 Owner(PrototypeCharacter)에서 참조
	class APrototypeCharacter* OwnerChar = nullptr;
	class UCameraComponent* FollowCamera = nullptr;

	/* ---- 거리/플래그/스윙 파라미터 (원본 그대로) ---- */
	float GrabbedObjectDistance = 0.f;
	float GrabMaxDistance = 700.f;
	float GrabMinDistance = 200.f;
	float ScrollDistanceSpeed = 50.f;

	bool  bIsLineTraceHit = false;
	bool  bIsGrabbingNonPhysics = false;
	bool  bIsRightMouseButtonDown = false;

	FVector LineStart, LineEnd;
	FHitResult CachedHitResult;

	// 비물리
	FVector NonPhysicsHitLocation;
	float   NonPhysicsGrabDistance = 0.f;
	float   NonPhysicsDistanceReduceSpeed = 1500.f;

	// 로프 반경?충돌
	bool bNonPhysicsCollisionOccurred = false;

	// Miss Grab Effect
	FVector MissedGrabTarget;
	bool   bShowMissedGrabVisual = false;
	float  MissedGrabTimer = 0.f;
	float  MissedGrabDuration = 0.3f;

	// Swing
	UPROPERTY(EditAnywhere, Category = "Swing") float CorrectionStrength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Swing") float SwingForce = 1200.f;

	// 안전 위치 히스토리
	TArray<FVector> SafeLocationHistory;
	int32 SafeLocationFrameDelay = 5;
	int32 SafeLocationMaxFrames = 10;

	/* ---------------- 내부 메서드 (본문은 .cpp에 원본 그대로) ---------------- */
	void RotateCharacterToMouse();
	void UpdateLineTrace();
	void UpdateGrabbedPhysics(float DeltaTime);
	void UpdateGrabbedNonPhysics(float DeltaTime);
	void UpdateMissedGrabVisual(float DeltaTime);
	void UpdateGrabVisualMesh();
};
