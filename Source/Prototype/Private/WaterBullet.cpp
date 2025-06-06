// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"
#include "Engine/Brush.h" // ABrush 클래스를 사용하기 위해 추가

// Sets default values
AWaterBullet::AWaterBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel10); // 물총알 전용 채널


	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 플레이어 무시
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Ignore); // 다른 물총알 무시
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Overlap); // 스펀지랑만 겹침
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // 적 겹침

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);


	// Projectile movement 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 700.f;
	ProjectileMovement->MaxSpeed = 700.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 영향 제거

	// LifeSpan은 헤더에 float LifeSpan = 3.0f; 와 같이 선언되어 있다고 가정합니다.
	// 생성자에서는 InitialLifeSpan을 직접 설정하거나, BeginPlay에서 SetLifeSpan을 사용합니다.
	// 여기서는 InitialLifeSpan = 3.0f; // 예시 값, 또는 헤더에 정의된 LifeSpan 변수 사용
	// 또는 아래처럼 헤더에 정의된 LifeSpan 변수를 사용한다고 가정합니다.
	// float LifeSpan = 3.0f; // 헤더에 선언된 변수라고 가정
	InitialLifeSpan = 3.0f; // 직접 값을 지정하거나, 헤더의 LifeSpan 변수를 사용하세요.
}

// Called when the game starts or when spawned
void AWaterBullet::BeginPlay()
{
	Super::BeginPlay();
	// SetLifeSpan(LifeSpan); // 생성자에서 InitialLifeSpan을 설정했다면 여기서 또 호출할 필요는 없을 수 있습니다.
	// 만약 LifeSpan 변수가 있고, 그 값을 BeginPlay 시점에 적용하고 싶다면 이 라인을 사용하세요.

	CollisionComp->OnComponentHit.AddDynamic(this, &AWaterBullet::OnHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWaterBullet::OnOverlapEnemy);

}

// Called every frame
void AWaterBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ProjectileMovement)
	{
		FVector Velocity = ProjectileMovement->Velocity;
		float Speed = Velocity.Size();

		// 최소 속도 제한
		float MinSpeed = 100.f;
		float Deceleration = 300.f; // 초당 줄어드는 속도량

		if (Speed > MinSpeed)
		{
			float NewSpeed = FMath::Max(Speed - Deceleration * DeltaTime, MinSpeed);
			ProjectileMovement->Velocity = Velocity.GetSafeNormal() * NewSpeed;
		}
		else if (!bIsDestroyTimerSet)
		{
			// 아직 타이머가 안 걸렸으면 0.2초 후 Destroy
			bIsDestroyTimerSet = true;
			GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AWaterBullet::DestroyBullet, 0.2f, false);
		}
	}
}
void AWaterBullet::DestroyBullet()
{
	// 파괴되기 전에 혹시 모를 타이머 정리
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	if (!IsPendingKill()) // 이미 파괴 중이 아니라면 파괴 호출
	{
		Destroy();
	}
}
void AWaterBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	// OtherActor가 유효한지 먼저 확인 (nullptr 방지)
	if (OtherActor && OtherActor != this)
	{
		if (Cast<ABrush>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("WaterBullet hit a Brush actor: %s. Destroying bullet."), *OtherActor->GetName());
			DestroyBullet(); // 수정된 DestroyBullet 호출
			return; // ABrush와 충돌 시 추가 로직 없이 종료
		}

		//다른 액터 타입과의 충돌 처리 로직을 추가
		UE_LOG(LogTemp, Warning, TEXT("WaterBullet hit: %s"), *OtherActor->GetName());
		DestroyBullet();
	}
}

void AWaterBullet::OnOverlapEnemy(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherActor가 유효한지 먼저 확인 (nullptr 방지)
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaterBullet overlapped with: %s"), *OtherActor->GetName());

		AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor);
		if (Enemy)
		{
			Enemy->Slowdown(2.f); // 1초 동안 느려짐
			DestroyBullet(); // 수정된 DestroyBullet 호출
		}
	}
}