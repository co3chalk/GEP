#include "WaterWeapon.h"
#include "WaterBullet.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PrototypeCharacter.h"

// 생성자
UWaterWeapon::UWaterWeapon()
{
    PrimaryComponentTick.bCanEverTick = true;
    FireInterval = 0.1f;
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

    FVector SpawnLocation = OwnerCharacter ? OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 500.f : FVector::ZeroVector;
    FRotator SpawnRotation = OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;

    AWaterBullet* Bullet = GetWorld()->SpawnActor<AWaterBullet>(WaterBulletClass, SpawnLocation, SpawnRotation);

}

// 연사 시작
void UWaterWeapon::StartFire()
{
    UE_LOG(LogTemp, Warning, TEXT("WaterWeapon StartFire called"));

    // 타이머로 연사
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UWaterWeapon::SpawnWater, FireInterval, true, 0.f);
}

// 연사 중지
void UWaterWeapon::StopFire()
{
    UE_LOG(LogTemp, Warning, TEXT("WaterWeapon StopFire called"));

    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}


