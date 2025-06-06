// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"
#include "Engine/Brush.h" // ABrush Ŭ������ ����ϱ� ���� �߰�

// Sets default values
AWaterBullet::AWaterBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel10); // ���Ѿ� ���� ä��


	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // �÷��̾� ����
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Ignore); // �ٸ� ���Ѿ� ����
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Overlap); // ���������� ��ħ
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // �� ��ħ

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);


	// Projectile movement ����
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 700.f;
	ProjectileMovement->MaxSpeed = 700.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // �߷� ���� ����

	// LifeSpan�� ����� float LifeSpan = 3.0f; �� ���� ����Ǿ� �ִٰ� �����մϴ�.
	// �����ڿ����� InitialLifeSpan�� ���� �����ϰų�, BeginPlay���� SetLifeSpan�� ����մϴ�.
	// ���⼭�� InitialLifeSpan = 3.0f; // ���� ��, �Ǵ� ����� ���ǵ� LifeSpan ���� ���
	// �Ǵ� �Ʒ�ó�� ����� ���ǵ� LifeSpan ������ ����Ѵٰ� �����մϴ�.
	// float LifeSpan = 3.0f; // ����� ����� ������� ����
	InitialLifeSpan = 3.0f; // ���� ���� �����ϰų�, ����� LifeSpan ������ ����ϼ���.
}

// Called when the game starts or when spawned
void AWaterBullet::BeginPlay()
{
	Super::BeginPlay();
	// SetLifeSpan(LifeSpan); // �����ڿ��� InitialLifeSpan�� �����ߴٸ� ���⼭ �� ȣ���� �ʿ�� ���� �� �ֽ��ϴ�.
	// ���� LifeSpan ������ �ְ�, �� ���� BeginPlay ������ �����ϰ� �ʹٸ� �� ������ ����ϼ���.

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

		// �ּ� �ӵ� ����
		float MinSpeed = 100.f;
		float Deceleration = 300.f; // �ʴ� �پ��� �ӵ���

		if (Speed > MinSpeed)
		{
			float NewSpeed = FMath::Max(Speed - Deceleration * DeltaTime, MinSpeed);
			ProjectileMovement->Velocity = Velocity.GetSafeNormal() * NewSpeed;
		}
		else if (!bIsDestroyTimerSet)
		{
			// ���� Ÿ�̸Ӱ� �� �ɷ����� 0.2�� �� Destroy
			bIsDestroyTimerSet = true;
			GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AWaterBullet::DestroyBullet, 0.2f, false);
		}
	}
}
void AWaterBullet::DestroyBullet()
{
	// �ı��Ǳ� ���� Ȥ�� �� Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	if (!IsPendingKill()) // �̹� �ı� ���� �ƴ϶�� �ı� ȣ��
	{
		Destroy();
	}
}
void AWaterBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	// OtherActor�� ��ȿ���� ���� Ȯ�� (nullptr ����)
	if (OtherActor && OtherActor != this)
	{
		if (Cast<ABrush>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("WaterBullet hit a Brush actor: %s. Destroying bullet."), *OtherActor->GetName());
			DestroyBullet(); // ������ DestroyBullet ȣ��
			return; // ABrush�� �浹 �� �߰� ���� ���� ����
		}

		//�ٸ� ���� Ÿ�԰��� �浹 ó�� ������ �߰�
		UE_LOG(LogTemp, Warning, TEXT("WaterBullet hit: %s"), *OtherActor->GetName());
		DestroyBullet();
	}
}

void AWaterBullet::OnOverlapEnemy(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherActor�� ��ȿ���� ���� Ȯ�� (nullptr ����)
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaterBullet overlapped with: %s"), *OtherActor->GetName());

		AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor);
		if (Enemy)
		{
			Enemy->Slowdown(2.f); // 1�� ���� ������
			DestroyBullet(); // ������ DestroyBullet ȣ��
		}
	}
}