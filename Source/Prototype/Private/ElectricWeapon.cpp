#include "ElectricWeapon.h"
#include "ElecBullet.h"
#include "PrototypeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyAnimInstance.h" // UMyAnimInstance를 사용하기 위해 포함


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
    if (!BulletClass || !OwnerChar) return;
    // **새로 추가/수정할 부분: 애니메이션 재생 로직**
    UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        UMyAnimInstance* MyAnim = Cast<UMyAnimInstance>(AnimInstance);
        if (MyAnim)
        {
            MyAnim->PlayfireMontage(); // 캐릭터의 애니메이션 몽타주 재생
            UE_LOG(LogTemp, Log, TEXT("UElectricWeapon::Fire() -> MyAnim->PlayfireMontage() called!")); // 로그 추가
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UElectricWeapon: AnimInstance is not of type UMyAnimInstance!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UElectricWeapon: No AnimInstance found on character mesh!"));
    }
    FVector Muzzle = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * 100.f;
    FRotator Dir = OwnerChar->GetActorRotation();

    GetWorld()->SpawnActor<AElecBullet>(BulletClass, Muzzle, Dir);
}
