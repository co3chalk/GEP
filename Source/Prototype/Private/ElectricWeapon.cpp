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
    /* ���� ���� = ĳ���Ͱ� �ٶ󺸴� ����(=Shooter ���) */
    // ĳ���� ȸ���� Shooter�� �̹� ó���ϰ� �����Ƿ� ���� �ڵ� ���ʿ�
}

void UElectricWeapon::Fire()
{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerChar->GetActorLocation());
    if (!BulletClass || !OwnerChar) return;

    FVector Muzzle = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * 100.f;
    FRotator Dir = OwnerChar->GetActorRotation();

    GetWorld()->SpawnActor<AElecBullet>(BulletClass, Muzzle, Dir);
}
