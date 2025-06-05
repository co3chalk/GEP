#include "WaterWeapon.h"
#include "WaterBullet.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "PrototypeCharacter.h"

// ������
UWaterWeapon::UWaterWeapon()
{
    PrimaryComponentTick.bCanEverTick = true;
    FireInterval = 0.05f;
}

// ������ ��
void UWaterWeapon::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<APrototypeCharacter>(GetOwner());
}

// �� �Ѿ� �߻� �Լ� (�ѹ� �߻�)
void UWaterWeapon::SpawnWater()
{
    if (!WaterBulletClass || !GetWorld())
        return;

    FVector Forward = OwnerCharacter->GetActorForwardVector();
    FVector Right = OwnerCharacter->GetActorRightVector();

    FVector SpawnLocation = OwnerCharacter ? OwnerCharacter->GetActorLocation() + Forward * 100.f + Right * 18.f : FVector::ZeroVector;
    FRotator SpawnRotation = OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;

    float RandomYawOffset = FMath::FRandRange(-5.0f, 5.0f);
    SpawnRotation.Yaw += RandomYawOffset;

    AWaterBullet* Bullet = GetWorld()->SpawnActor<AWaterBullet>(WaterBulletClass, SpawnLocation, SpawnRotation);

}

// ���� ����
void UWaterWeapon::StartFire() {
    UE_LOG(LogTemp, Warning, TEXT("WaterWeapon StartFire called"));

    const FString SoundAssetPath = TEXT("/Game/Audio/water1");
    USoundBase* SoundToPlay = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *SoundAssetPath));

    if (SoundToPlay && GetWorld()) {
        FVector SoundLocation = FVector::ZeroVector;
        if (GetOwner()) {
            SoundLocation = GetOwner()->GetActorLocation();
        }
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, SoundLocation);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load sound '%s' or GetWorld() is null."), *SoundAssetPath);
    }

    if (GetWorld()) {
        GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UWaterWeapon::SpawnWater, FireInterval, true, 0.f);
    }
}

// ���� ����
void UWaterWeapon::StopFire()
{
    UE_LOG(LogTemp, Warning, TEXT("WaterWeapon StopFire called"));

    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}


