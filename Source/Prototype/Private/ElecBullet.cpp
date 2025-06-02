// ElecBullet.cpp
#include "ElecBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "EnemyActor.h"
#include "Battery.h"
#include "NiagaraFunctionLibrary.h"

AElecBullet::AElecBullet()
{
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(12.f);
    // OnComponentHit 바인딩 시 UFUNCTION() 매크로가 붙은 함수와 시그니처가 일치해야 합니다.
    // 헤더 파일의 OnHit 함수 선언에 UFUNCTION()이 있고, 파라미터가 올바른지 확인하세요.
    Collision->OnComponentHit.AddDynamic(this, &AElecBullet::OnHit);

    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_GameTraceChannel1);    // "Bullet" 채널

    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 이미 Pawn을 무시하도록 설정되어 있음
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // Enemy 전용
    Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = 2000.f;
    Movement->MaxSpeed = 3000.f;
    Movement->bRotationFollowsVelocity = true;
    Movement->ProjectileGravityScale = 0.0f;

    ElecEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricEffect"));
    ElecEffect->SetupAttachment(RootComponent);
    ElecEffect->SetAutoActivate(true);

    InitialLifeSpan = 8.f;
}
void AElecBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // --- 시작: OtherActor 유효성 검사 ---
    if (!OtherActor)
    {
        UE_LOG(LogTemp, Error, TEXT("AElecBullet::OnHit - OtherActor is NULL. Bullet: %s, HitComponent (Bullet's): %s."), *GetNameSafe(this), *GetNameSafe(HitComponent));
        Destroy(); // 총알 파괴
        return;
    }

    // 총알이 자기 자신과 충돌한 경우 (스폰 직후 등)
    if (OtherActor == this)
    {
        // 이 경우는 보통 무시합니다. 필요에 따라 로그를 남길 수 있습니다.
        // ProjectileMovementComponent의 bInitialVelocityInLocalSpace = false 이거나, 스폰 위치 조정으로 방지하는 것이 좋습니다.
        UE_LOG(LogTemp, Verbose, TEXT("AElecBullet::OnHit - Bullet hit itself. Ignoring. Bullet: %s"), *GetNameSafe(this));
        return; // 자기 자신과의 충돌은 특별한 처리를 하지 않고 반환 (파괴 X)
    }

    if (OtherActor->IsPendingKill())
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - OtherActor '%s' (Class: %s) is pending kill. Ignoring hit. Bullet: %s"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()), *GetNameSafe(this));
        Destroy(); // 총알 파괴
        return;
    }
    // --- 종료: OtherActor 유효성 검사 ---

    // 이제 OtherActor는 유효함 (nullptr 아님, IsPendingKill 아님, 자기 자신 아님)

    // 충돌 시 이펙트 생성
    if (ImpactEffect) // UNiagaraSystem* 타입의 UPROPERTY 변수
    {
        // GetActorLocation() 대신 Hit.ImpactPoint를, FRotator::ZeroRotator 대신 Hit.ImpactNormal.Rotation()을 사용하면 더 정확한 위치/방향에 이펙트가 생성됩니다.
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactEffect,
            Hit.ImpactPoint, // 충돌 지점
            Hit.ImpactNormal.Rotation(), // 충돌 표면의 법선 방향으로 회전
            FVector(1.f), // 스케일
            true,         // 자동 소멸
            true,         // 자동 활성화
            ENCPoolMethod::None,
            true          // 월드 공간에서 풀링 (bAutoActivate가 true이면 이 파라미터가 실제 활성화를 제어)
        );

        if (NiagaraComp == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("AElecBullet::OnHit - Failed to spawn NiagaraComponent for ImpactEffect. ImpactEffect asset might be invalid. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
        }
        else
        {
            // SpawnSystemAtLocation의 마지막 파라미터 (bAutoActivate)가 true이면 NiagaraComp->Activate(true)는 보통 필요 없습니다.
            UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Spawned Niagara ImpactEffect. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
        }
    }
    else if (ImpactFX) // UParticleSystem* 타입의 UPROPERTY 변수
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ImpactFX,
            Hit.ImpactPoint,
            Hit.ImpactNormal.Rotation(),
            FVector(1.f),
            true
        );
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Spawned ParticleSystem ImpactFX. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AElecBullet::OnHit - No ImpactEffect (Niagara) or ImpactFX (Particle) assigned. Bullet: %s, Target: %s"), *GetNameSafe(this), *OtherActor->GetName());
    }

    // OtherActor 타입에 따른 처리 (이제 OtherActor->GetName() 등은 안전하게 사용 가능)
    if (AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor))
    {
        Enemy->Freeze(3.0f); // Freeze 함수 내부에서도 Enemy 유효성 검사가 필요할 수 있음
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Enemy: %s (Class: %s)"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()));
    }
    else if (ABattery* Battery = Cast<ABattery>(OtherActor))
    {
        Battery->Charge(); // Charge 함수 내부에서도 Battery 유효성 검사가 필요할 수 있음
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit Battery: %s (Class: %s)"), *OtherActor->GetName(), *GetNameSafe(OtherActor->GetClass()));
    }
    else
    {
        // 기타 액터 (예: 벽, 바닥 등 WorldStatic, WorldDynamic 객체) 와 충돌한 경우
        // 콜리전 설정에 따라 이 경우는 정상적인 동작일 수 있습니다.
        UE_LOG(LogTemp, Log, TEXT("AElecBullet::OnHit - Hit other actor: %s (Class: %s), Hit Component: %s (Class: %s)"),
            *OtherActor->GetName(),
            *GetNameSafe(OtherActor->GetClass()),
            *GetNameSafe(OtherComp), // 상대방의 어떤 컴포넌트에 맞았는지
            *GetNameSafe(OtherComp ? OtherComp->GetClass() : nullptr)
        );
    }

    // 총알 파괴 (모든 상호작용 후)
    Destroy();
}