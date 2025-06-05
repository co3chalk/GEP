#include "WaterWeapon.h"
#include "WaterBullet.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "PrototypeCharacter.h"

// 생성자
UWaterWeapon::UWaterWeapon()
{
    PrimaryComponentTick.bCanEverTick = true;
    FireInterval = 0.05f;
}

// 시작할 때
void UWaterWeapon::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<APrototypeCharacter>(GetOwner());
}

// 물 총알 발사 함수 (한발 발사)
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

// 연사 시작
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

// 연사 중지
void UWaterWeapon::StopFire()
{
    UE_LOG(LogTemp, Warning, TEXT("WaterWeapon StopFire called"));

    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}


