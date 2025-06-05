// GEP.zip/Source/Prototype/Private/Shooter.cpp

#include "Shooter.h"
#include "PrototypeCharacter.h"
#include "EnemyActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"

// --- 아래 헤더 파일을 추가합니다 ---
#include "PrototypeGameMode.h" // APrototypeGameMode 클래스를 사용하기 위함
// ---------------------------------

// ... (UShooter 생성자, BeginPlay, TickComponent, GetActualLineTraceStartLocation, GetVisualCylinderStartLocation 함수들은 그대로 유지) ...
// (이전 코드와 동일하게 유지됩니다)
UShooter::UShooter()
{
	PrimaryComponentTick.bCanEverTick = true;
	NonPhysicsGrabRotationInterpSpeed = 360.0f;
	MaxGrabStretchTolerance = 50.0f;
	GrabDistanceAdaptThreshold = 40.0f;

	GrabVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrabVisualMesh"));
	GrabVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrabVisualMesh->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Game/Meshes/Cube.Cube"));
	if (MeshAsset.Succeeded())
		GrabVisualMesh->SetStaticMesh(MeshAsset.Object);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	RotationConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RotationConstraint"));
}

void UShooter::BeginPlay()
{
	Super::BeginPlay();

	OwnerChar = Cast<APrototypeCharacter>(GetOwner());
	FollowCamera = OwnerChar ? OwnerChar->FindComponentByClass<UCameraComponent>() : nullptr;

	if (OwnerChar)
	{
		GrabVisualMesh->AttachToComponent(
			OwnerChar->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
		RotationConstraint->AttachToComponent(
			OwnerChar->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);

		GrabVisualMesh->RegisterComponent();
		PhysicsHandle->RegisterComponent();
		RotationConstraint->RegisterComponent();
	}

	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}


/*-------------------------- Tick ---------------------------*/
void UShooter::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTick)
{
	Super::TickComponent(DeltaTime, TickType, ThisTick);

	if (!OwnerChar) return;

	UpdateLineTrace();
	UpdateGrabbedPhysics(DeltaTime);
	UpdateGrabbedNonPhysics(DeltaTime);
	UpdateMissedGrabVisual(DeltaTime);
}

FVector UShooter::GetActualLineTraceStartLocation() const
{
	if (!OwnerChar) { /*...*/ return FVector::ZeroVector; }
	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);
	if (ProtoChar)
	{
		if (ProtoChar->GetMesh() && ProtoChar->CharacterMuzzleSocketName != NAME_None)
		{
			if (ProtoChar->GetMesh()->DoesSocketExist(ProtoChar->CharacterMuzzleSocketName))
			{
				return ProtoChar->GetMesh()->GetSocketLocation(ProtoChar->CharacterMuzzleSocketName);
			}
			return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f);
		}
		return OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f);
	}
	return OwnerChar ? OwnerChar->GetActorLocation() + FVector(0, 0, 50.0f) : FVector::ZeroVector;
}

FVector UShooter::GetVisualCylinderStartLocation() const
{
	if (!OwnerChar)
	{
		if (GetOwner()) return GetOwner()->GetActorLocation();
		return FVector::ZeroVector;
	}

	APrototypeCharacter* ProtoChar = Cast<APrototypeCharacter>(OwnerChar);

	if (ProtoChar && ProtoChar->AttachedBusterMeshComponent)
	{
		UMeshComponent* ActualBusterMesh = Cast<UMeshComponent>(ProtoChar->AttachedBusterMeshComponent);
		if (ActualBusterMesh && ProtoChar->NozzleSocketNameOnBusterMesh != NAME_None && ActualBusterMesh->DoesSocketExist(ProtoChar->NozzleSocketNameOnBusterMesh))
		{
			FVector NozzleLocation = ActualBusterMesh->GetSocketLocation(ProtoChar->NozzleSocketNameOnBusterMesh);
			return NozzleLocation;
		}
		if (ActualBusterMesh)
		{
			return ActualBusterMesh->GetComponentLocation();
		}
	}
	return GetActualLineTraceStartLocation();
}


void UShooter::Grab() {
	UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!!!!!! UShooter::Grab() FUNCTION HAS BEEN CALLED !!!!!!!!!!!!!!"));

	// --- 사운드 재생 부분 수정 ---
	const FString SoundAssetPath = TEXT("/Game/Audio/wire3"); // 이전과 동일한 경로
	USoundBase* SoundToPlay = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *SoundAssetPath));

	if (SoundToPlay && GetWorld() && OwnerChar) { // GetWorld()와 OwnerChar 유효성 검사 추가
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, OwnerChar->GetActorLocation());
	}
	else {
		if (!SoundToPlay) {
			UE_LOG(LogTemp, Warning, TEXT("UShooter::Grab - Failed to load sound: %s"), *SoundAssetPath);
		}
		if (!OwnerChar) {
			UE_LOG(LogTemp, Warning, TEXT("UShooter::Grab - OwnerChar is null, cannot get location for sound."));
		}
		// GetWorld()가 null인 경우는 PlaySoundAtLocation 호출 자체가 불가능하므로, UE_LOG는 생략하거나 추가할 수 있습니다.
	}
	// --- 사운드 재생 부분 수정 완료 ---

	if (bIsLineTraceHit && CachedHitResult.GetComponent()) {
		UPrimitiveComponent* HitComponent = CachedHitResult.GetComponent();
		AActor* HitActor = CachedHitResult.GetActor();

		if (!HitActor) {
			UE_LOG(LogTemp, Warning, TEXT("UShooter::Grab - HitComponent '%s' (Class: %s) did not return a valid Actor. Grab attempt aborted."),
				*GetNameSafe(HitComponent),
				*GetNameSafe(HitComponent->GetClass())
			);
			MissedGrabTarget = LineEnd;
			MissedGrabTimer = MissedGrabDuration;
			bShowMissedGrabVisual = true;
			return;
		}

		FString ActorName = HitActor->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Shooter: Attempting to grab '%s' (Class: %s). Tags found: %d. Tags: [...]"),
			*ActorName,
			*HitActor->GetClass()->GetName(),
			HitActor->Tags.Num()
		);

		if (HitActor->Tags.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("Shooter: Object '%s' has tag(s). Grab ignored."), *HitActor->GetName());
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Shooter: Object '%s' has NO tags. Proceeding with grab."), *ActorName);

		if (HitComponent->IsSimulatingPhysics()) {
			APrototypeGameMode* GameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld())); // GetWorld() 유효성 검사가 필요할 수 있음
			if (GameMode && !GameMode->IsPysGrabActiveForLevel()) {
				UE_LOG(LogTemp, Warning, TEXT("Shooter: 물리 그랩 기능이 GameMode에서 활성화되지 않았습니다. 그랩 시도 무시."));
				MissedGrabTarget = LineEnd;
				MissedGrabTimer = MissedGrabDuration;
				bShowMissedGrabVisual = true;
				return;
			}
			else if (!GameMode) {
				UE_LOG(LogTemp, Error, TEXT("Shooter: GameMode를 가져올 수 없습니다. 물리 그랩 시도 실패."));
				return;
			}
			UE_LOG(LogTemp, Log, TEXT("Shooter: GameMode에서 물리 그랩 기능 활성화됨. 물리 그랩 진행."));

			FVector GrabStartPointForDistanceCalc = OwnerChar->GetActorLocation();
			float CurrentDistance = FVector::Dist(GrabStartPointForDistanceCalc, HitComponent->GetComponentLocation());
			if (CurrentDistance < GrabMinDistance || CurrentDistance > GrabMaxDistance) {
				UE_LOG(LogTemp, Warning, TEXT("Shooter: 물리 오브젝트가 그랩 가능 거리를 벗어났습니다."));
				return;
			}

			HitComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			HitComponent->SetEnableGravity(false);

			GrabbedObjectDistance = CurrentDistance;
			GrabbedComponent = HitComponent;

			PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetComponentLocation());
			PhysicsHandle->InterpolationSpeed = 100.0f;
			PhysicsHandle->SetTargetLocation(GrabStartPointForDistanceCalc + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance);

			RotationConstraint->SetWorldLocation(HitComponent->GetComponentLocation());
			RotationConstraint->SetConstrainedComponents(nullptr, NAME_None, HitComponent, NAME_None);
			RotationConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.f);
			RotationConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
			RotationConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);

			DrawDebugSphere(GetWorld(), HitComponent->GetComponentLocation(), 20.f, 12, FColor::Yellow, false, 1.0f);
			if (OwnerChar && OwnerChar->GetCharacterMovement()) {
				OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false;
			}
		}
		else {
			if (OwnerChar && OwnerChar->GetCharacterMovement()) {
				FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
				CurrentVelocity.X = 0.0f;
				CurrentVelocity.Y = 0.0f;
				OwnerChar->GetCharacterMovement()->Velocity = CurrentVelocity;
				OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false;
			}
			NonPhysicsHitLocation = CachedHitResult.ImpactPoint;
			FVector GrabStartPointForDistanceCalc = OwnerChar->GetActorLocation();
			NonPhysicsGrabDistance = FVector::Dist(GrabStartPointForDistanceCalc, NonPhysicsHitLocation);
			bIsGrabbingNonPhysics = true;
		}
	}
	else {
		MissedGrabTarget = LineEnd;
		MissedGrabTimer = MissedGrabDuration;
		bShowMissedGrabVisual = true;
	}
}

void UShooter::Release() {
	if (GrabbedComponent)
	{
		GrabbedComponent->SetEnableGravity(true);
		GrabbedComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		GrabbedComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		RotationConstraint->BreakConstraint();
		PhysicsHandle->ReleaseComponent();
		GrabbedComponent = nullptr;

	}
	if (bIsGrabbingNonPhysics) {
		FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = 0.0f;

	}
	bIsGrabbingNonPhysics = false;
	OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
	if (GrabVisualMesh)
	{
		GrabVisualMesh->SetVisibility(false);
	}
	bool bWasGrabbing = GrabbedComponent || bIsGrabbingNonPhysics;

	GrabbedComponent = nullptr;
	bIsGrabbingNonPhysics = false;
	bShowMissedGrabVisual = false;
	bNonPhysicsCollisionOccurred = false;

	if (OwnerChar && OwnerChar->GetCharacterMovement())
	{
		OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;
		if (bWasGrabbing)
		{
			OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}

}

void UShooter::ScrollUp() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance + ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);
		UpdateGrabVisualMesh();
	}
}
void UShooter::ScrollDown() {
	if (GrabbedComponent)
	{
		GrabbedObjectDistance = FMath::Clamp(GrabbedObjectDistance - ScrollDistanceSpeed, GrabMinDistance, GrabMaxDistance);
		FVector TargetLocation = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
		PhysicsHandle->SetTargetLocation(TargetLocation);
		UpdateGrabVisualMesh();
	}
}
void UShooter::RightMouseDown() { bIsRightMouseButtonDown = true; }
void UShooter::RightMouseUp() { bIsRightMouseButtonDown = false; }

void UShooter::UpdateLineTrace()
{
	if (!OwnerChar) return;

	LineStart = GetActualLineTraceStartLocation();
	FVector CharacterForwardDirection = OwnerChar->GetActorForwardVector();
	LineEnd = LineStart + CharacterForwardDirection * GrabMaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerChar);
	if (CachedHitResult.GetActor() && CachedHitResult.GetActor()->ActorHasTag("Enemy"))
	{
		Params.AddIgnoredActor(CachedHitResult.GetActor());
	}

	bIsLineTraceHit = GetWorld()->LineTraceSingleByChannel(
		CachedHitResult, LineStart, LineEnd, ECC_PhysicsBody, Params);

	FColor LineColor = bIsLineTraceHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, false, 0.0f, 0, 2.0f);
}


void UShooter::UpdateGrabbedPhysics(float DeltaTime)
{
	if (!GrabbedComponent || !OwnerChar) return;
	float CurrentActualDistanceToObject = FVector::Dist(OwnerChar->GetActorLocation(), GrabbedComponent->GetComponentLocation());
	if (CurrentActualDistanceToObject < (GrabbedObjectDistance - GrabDistanceAdaptThreshold))
	{
		GrabbedObjectDistance = CurrentActualDistanceToObject;
		GrabbedObjectDistance = FMath::Max(GrabbedObjectDistance, GrabMinDistance);
		UE_LOG(LogTemp, Log, TEXT("GrabbedObjectDistance adapted due to significant proximity: %f"), GrabbedObjectDistance);
	}
	FVector AimCheckStartLocation = GetActualLineTraceStartLocation();
	FVector AimDirection = OwnerChar->GetActorForwardVector();

	float AimCheckLength = GrabbedObjectDistance + MaxGrabStretchTolerance;
	if (AimCheckLength < GrabMinDistance) AimCheckLength = GrabMinDistance;

	FVector AimCheckEndLocation = AimCheckStartLocation + AimDirection * AimCheckLength;

	FHitResult AimHitResult;
	FCollisionQueryParams AimParams;
	AimParams.AddIgnoredActor(OwnerChar);
	const float AimCheckRadius = 25.0f;
	FCollisionShape SphereToSweep = FCollisionShape::MakeSphere(AimCheckRadius);

	bool bAimHitSomething = GetWorld()->SweepSingleByChannel(
		AimHitResult,
		AimCheckStartLocation,
		AimCheckEndLocation,
		FQuat::Identity,
		ECC_PhysicsBody,
		SphereToSweep,
		AimParams);

	bool bStillAimingAtGrabbedObject = false;
	if (bAimHitSomething)
	{
		if (AimHitResult.GetComponent() == GrabbedComponent)
		{
			bStillAimingAtGrabbedObject = true;
		}
	}
	if (GetWorld())
	{
		FColor DebugColor = bStillAimingAtGrabbedObject ? FColor::Cyan : FColor::Orange;
		DrawDebugLine(GetWorld(), AimCheckStartLocation, AimCheckEndLocation, DebugColor, false, -1.0f, 0, 1.f);
	}


	if (!bStillAimingAtGrabbedObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooter: Aim (SphereTrace) moved off grabbed object. Releasing."));
		Release();
		return;
	}
	FVector TargetLocation = OwnerChar->GetActorLocation() +
		OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;

	PhysicsHandle->InterpolationSpeed = 100.0f;
	PhysicsHandle->SetTargetLocation(TargetLocation);
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = TargetLocation;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
			return;
		}
		FVector Mid = (Start + End) * 0.5f;
		float ConstantMyThickness = 0.1f;
		float ScaleZ = Length / 100.0f;

		GrabVisualMesh->SetVisibility(true);
		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(ConstantMyThickness, ConstantMyThickness, ScaleZ));
	}
}
void UShooter::UpdateGrabbedNonPhysics(float DeltaTime)
{
	if (!bIsGrabbingNonPhysics || !OwnerChar) return;

	FVector Delta = OwnerChar->GetActorLocation() - NonPhysicsHitLocation;
	OwnerChar->GetCharacterMovement()->GravityScale = 1.0f;

	FVector RopeVector = Delta;
	float CurrentDistance = RopeVector.Size();
	FVector Direction = RopeVector.GetSafeNormal();
	FVector TargetLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;

	FVector IdealLocation = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
	FVector CorrectionVector = IdealLocation - OwnerChar->GetActorLocation();
	FVector CurrentVelocity = OwnerChar->GetCharacterMovement()->Velocity;
	CurrentVelocity += CorrectionVector * CorrectionStrength * DeltaTime;

	if (OwnerChar->Controller)
	{
		FVector Forward = OwnerChar->Controller->GetControlRotation().Vector();
		FVector Right = FRotationMatrix(OwnerChar->Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);

		FVector InputVector =
			Forward * OwnerChar->GetInputAxisValue("MoveForward") +
			Right * OwnerChar->GetInputAxisValue("MoveRight");

		InputVector.Z = 0.0f;
		CurrentVelocity += InputVector * SwingForce * DeltaTime;
	}

	OwnerChar->GetCharacterMovement()->Velocity = CurrentVelocity;

	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = NonPhysicsHitLocation;

		FVector VisualRopeDir = End - Start;
		float Length = VisualRopeDir.Size();
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
		}
		else
		{
			FVector Mid = (Start + End) * 0.5f;
			FVector CameraDir = FollowCamera ? FollowCamera->GetForwardVector() : OwnerChar->GetActorForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
				FVector::DotProduct(CameraDir, VisualRopeDir.GetSafeNormal())));
			float ConstantThickness = 0.1f;
			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(VisualRopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}
	}

	if (bIsRightMouseButtonDown)
	{
		NonPhysicsGrabDistance -= NonPhysicsDistanceReduceSpeed * DeltaTime;
		NonPhysicsGrabDistance = FMath::Max(NonPhysicsGrabDistance, 100.0f);

		FVector PullTarget = NonPhysicsHitLocation + Direction * NonPhysicsGrabDistance;
		FVector NewLocation = FMath::VInterpTo(OwnerChar->GetActorLocation(), PullTarget, DeltaTime, 800.0f);
		OwnerChar->SetActorLocation(NewLocation, true);
	}
	else
	{
		FVector NewLocation = FMath::VInterpTo(OwnerChar->GetActorLocation(), TargetLocation, DeltaTime, 100.0f);
		OwnerChar->SetActorLocation(NewLocation, true);
	}

	float MaxAllowedDistance = NonPhysicsGrabDistance;
	FVector DesiredLocation = OwnerChar->GetActorLocation();
	FVector OffsetFromCenter = DesiredLocation - NonPhysicsHitLocation;
	float OffsetLength = OffsetFromCenter.Size();

	if (OffsetLength > MaxAllowedDistance)
	{
		OffsetFromCenter = OffsetFromCenter.GetSafeNormal() * MaxAllowedDistance;
		FVector CorrectedLocation = NonPhysicsHitLocation + OffsetFromCenter;
		OwnerChar->SetActorLocation(CorrectedLocation, false);
	}
	if (OwnerChar->GetCharacterMovement() && !OwnerChar->GetCharacterMovement()->bOrientRotationToMovement)
	{
		FVector TargetLookAtLocation = NonPhysicsHitLocation;
		FVector CharacterLocation = OwnerChar->GetActorLocation();

		FVector DirectionToTarget = TargetLookAtLocation - CharacterLocation;
		DirectionToTarget.Z = 0.0f;

		if (!DirectionToTarget.IsNearlyZero(0.01f))
		{
			FRotator CurrentRotation = OwnerChar->GetActorRotation();
			FRotator TargetRotation = DirectionToTarget.Rotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, NonPhysicsGrabRotationInterpSpeed);
			OwnerChar->SetActorRotation(NewRotation);
		}
	}
	if (GrabVisualMesh)
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = NonPhysicsHitLocation;

		FVector VisualRopeDir = End - Start;
		float Length = VisualRopeDir.Size();
		if (Length < KINDA_SMALL_NUMBER)
		{
			GrabVisualMesh->SetVisibility(false);
		}
		else
		{
			FVector Mid = (Start + End) * 0.5f;
			FVector CameraDir = FollowCamera ? FollowCamera->GetForwardVector() : OwnerChar->GetActorForwardVector();
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
				FVector::DotProduct(CameraDir, VisualRopeDir.GetSafeNormal())));
			float ConstantThickness = 0.1f;
			float ScaleZ = Length / 100.0f;

			GrabVisualMesh->SetVisibility(true);
			GrabVisualMesh->SetWorldLocation(Mid);
			GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(VisualRopeDir).Rotator());
			GrabVisualMesh->SetWorldScale3D(FVector(ConstantThickness, ConstantThickness, ScaleZ));
		}
	}
	DrawDebugLine(GetWorld(), OwnerChar->GetActorLocation(), NonPhysicsHitLocation, FColor::Purple, false, 0.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, 15.0f, 12, FColor::Yellow, false, 0.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), NonPhysicsHitLocation, MaxAllowedDistance, 32, FColor::Green, false, 0.0f, 0, 0.5f);
}
void UShooter::UpdateMissedGrabVisual(float DeltaTime)
{
	if (!bShowMissedGrabVisual || !GrabVisualMesh || !OwnerChar) return;

	MissedGrabTimer -= DeltaTime;

	if (MissedGrabTimer <= 0.0f)
	{
		bShowMissedGrabVisual = false;
		GrabVisualMesh->SetVisibility(false);
	}
	else
	{
		FVector Start = GetVisualCylinderStartLocation();
		FVector End = MissedGrabTarget;
		FVector RopeDir = End - Start;
		float Length = RopeDir.Size();
		FVector Mid = (Start + End) * 0.5f;

		FVector CameraDir = FollowCamera->GetForwardVector();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
			FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
		float Thickness = 0.1f;

		float ScaleZ = Length / 100.0f;

		GrabVisualMesh->SetWorldLocation(Mid);
		GrabVisualMesh->SetWorldRotation(FRotationMatrix::MakeFromZ(RopeDir).Rotator());
		GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
		GrabVisualMesh->SetVisibility(true);
	}
}
void UShooter::UpdateGrabVisualMesh()
{
	if (!GrabVisualMesh || !GrabbedComponent || !OwnerChar) return;

	FVector Start = GetVisualCylinderStartLocation();
	FVector End = Start + OwnerChar->GetActorForwardVector() * GrabbedObjectDistance;
	FVector RopeDir = End - Start;
	float Length = RopeDir.Size();
	FVector Mid = (Start + End) * 0.5f;

	float ScaleZ = Length / 100.0f;
	FRotator Rotation = FRotationMatrix::MakeFromZ(RopeDir).Rotator();

	FVector CameraDir = FollowCamera->GetForwardVector();
	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(
		FVector::DotProduct(CameraDir, RopeDir.GetSafeNormal())));
	float Thickness = (AngleDeg < 15.0f) ? 3.0f : 0.2f;

	GrabVisualMesh->SetWorldLocation(Mid);
	GrabVisualMesh->SetWorldRotation(Rotation);
	GrabVisualMesh->SetWorldScale3D(FVector(Thickness, Thickness, ScaleZ));
	GrabVisualMesh->SetVisibility(true);
}