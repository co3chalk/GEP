#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter.generated.h"

class UPhysicsHandleComponent;
class UPhysicsConstraintComponent;
class UStaticMeshComponent;

/**
 * Grab / Rope-Swing / Scroll / ��Ŭ�� ���� ���� ������Ʈ
 *  ? APrototypeCharacter �ȿ� �ִ� ���� ������ �̵�
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UShooter : public UActorComponent
{
	GENERATED_BODY()

public:
	UShooter();

	/*------------- APrototypeCharacter ���� ȣ���� ���� -------------*/
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
	/* ====== APrototypeCharacter �� �ִ� Grab ���� ����� ������ ���� �̵� ====== */
	UPROPERTY() UPhysicsHandleComponent* PhysicsHandle;
	UPROPERTY() UPhysicsConstraintComponent* RotationConstraint;
	UPROPERTY() UStaticMeshComponent* GrabVisualMesh;
	UPROPERTY() UPrimitiveComponent* GrabbedComponent = nullptr;

	// ī�޶�� Owner(PrototypeCharacter)���� ����
	class APrototypeCharacter* OwnerChar = nullptr;
	class UCameraComponent* FollowCamera = nullptr;

	/* ---- �Ÿ�/�÷���/���� �Ķ���� (���� �״��) ---- */
	float GrabbedObjectDistance = 0.f;
	float GrabMaxDistance = 700.f;
	float GrabMinDistance = 200.f;
	float ScrollDistanceSpeed = 50.f;

	bool  bIsLineTraceHit = false;
	bool  bIsGrabbingNonPhysics = false;
	bool  bIsRightMouseButtonDown = false;

	FVector LineStart, LineEnd;
	FHitResult CachedHitResult;

	// �񹰸�
	FVector NonPhysicsHitLocation;
	float   NonPhysicsGrabDistance = 0.f;
	float   NonPhysicsDistanceReduceSpeed = 1500.f;

	// ���� �ݰ�?�浹
	bool bNonPhysicsCollisionOccurred = false;

	// Miss Grab Effect
	FVector MissedGrabTarget;
	bool   bShowMissedGrabVisual = false;
	float  MissedGrabTimer = 0.f;
	float  MissedGrabDuration = 0.3f;

	// Swing
	UPROPERTY(EditAnywhere, Category = "Swing") float CorrectionStrength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Swing") float SwingForce = 1200.f;

	// ���� ��ġ �����丮
	TArray<FVector> SafeLocationHistory;
	int32 SafeLocationFrameDelay = 5;
	int32 SafeLocationMaxFrames = 10;

	/* ---------------- ���� �޼��� (������ .cpp�� ���� �״��) ---------------- */
	void RotateCharacterToMouse();
	void UpdateLineTrace();
	void UpdateGrabbedPhysics(float DeltaTime);
	void UpdateGrabbedNonPhysics(float DeltaTime);
	void UpdateMissedGrabVisual(float DeltaTime);
	void UpdateGrabVisualMesh();
};
