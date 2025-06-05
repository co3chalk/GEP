#include "ElectricWeapon.h"
#include "ElecBullet.h"
#include "PrototypeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

UElectricWeapon::UElectricWeapon()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UElectricWeapon::BeginPlay()
{
    Super::BeginPlay();
    OwnerChar = Cast<APrototypeCharacter>(GetOwner());
    FollowCamera = OwnerChar ? OwnerChar->FindComponentByClass<UCameraComponent>() : nullptr;
}

void UElectricWeapon::TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*)
{
    if (!OwnerChar || !FollowCamera) return;
    /* 무기 방향 = 캐릭터가 바라보는 방향(=Shooter 방식) */
    // 캐릭터 회전은 Shooter가 이미 처리하고 있으므로 별도 코드 불필요
}

void UElectricWeapon::Fire()
{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerChar->GetActorLocation());
    if (!BulletClass || !OwnerChar) return;

    FVector Muzzle = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * 100.f;
    FRotator Dir = OwnerChar->GetActorRotation();

    GetWorld()->SpawnActor<AElecBullet>(BulletClass, Muzzle, Dir);
}
