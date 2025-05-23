#include "ElectricWeapon.h"
#include "ElecBullet.h"
#include "PrototypeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyAnimInstance.h" // UMyAnimInstance�� ����ϱ� ���� ����


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
    if (!BulletClass || !OwnerChar) return;
    // **���� �߰�/������ �κ�: �ִϸ��̼� ��� ����**
    UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        UMyAnimInstance* MyAnim = Cast<UMyAnimInstance>(AnimInstance);
        if (MyAnim)
        {
            MyAnim->PlayfireMontage(); // ĳ������ �ִϸ��̼� ��Ÿ�� ���
            UE_LOG(LogTemp, Log, TEXT("UElectricWeapon::Fire() -> MyAnim->PlayfireMontage() called!")); // �α� �߰�
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
