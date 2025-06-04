// PortalTeleporterBase.cpp

#include "PortalTeleporterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h" // �÷��̾� ĳ���� ĳ������ ���� (�Ǵ� ����ϴ� Ư�� ĳ���� Ŭ����)
// #include "Kismet/GameplayStatics.h" // �ʿ�� ���

APortalTeleporterBase::APortalTeleporterBase()
{
    PrimaryActorTick.bCanEverTick = false; // Tick�� �ʿ� ���ٸ� ���Ӵϴ�.

    // ��Ʈ ������Ʈ ������ �� Ʈ���� ���� ����
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    if (TriggerVolume)
    {
        RootComponent = TriggerVolume;
        TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // ���� ���Ϳ� �������ǵ��� ����
        // TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f)); // �ʿ�� �⺻ ũ�� ����
    }

    // ��Ż �ð� �޽� ������Ʈ ���� (���� ����)
    PortalVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalVisualMesh"));
    if (PortalVisualMesh && RootComponent)
    {
        PortalVisualMesh->SetupAttachment(RootComponent); // ��Ʈ ������Ʈ�� ����
        PortalVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �ð����̹Ƿ� �ݸ��� ����
    }

    // ��ǥ ��ġ �⺻�� ����
    TargetWorldLocation = FVector::ZeroVector;
}

void APortalTeleporterBase::BeginPlay()
{
    Super::BeginPlay();

    // ������ �̺�Ʈ�� �Լ� ���ε�
    if (TriggerVolume)
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalTeleporterBase::OnPortalOverlap);
    }
}

void APortalTeleporterBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // �ڱ� �ڽ��̳� null ���ʹ� ����
    if (OtherActor && (OtherActor != this))
    {
        // �÷��̾� ĳ�������� Ȯ�� (�Ǵ� ���ϴ� �ٸ� ���� Ÿ������ ĳ����)
        ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor); // ��� ĳ���� Ÿ�� ���
        // �Ǵ� Ư�� �÷��̾� ĳ���ͷ� �����ϰ� �ʹٸ�:
        // AYourPlayerCharacter* PlayerCharacter = Cast<AYourPlayerCharacter>(OtherActor);

        if (PlayerCharacter)
        {
            UE_LOG(LogTemp, Log, TEXT("Player '%s' overlapped with portal. Teleporting to %s."), *PlayerCharacter->GetName(), *TargetWorldLocation.ToString());

            // ������ TargetWorldLocation���� ���� ��ġ ����
            // ETeleportType::TeleportPhysics�� �̵� �� ���� ���¸� �ʱ�ȭ�Ͽ� �������� ���Դϴ�.
            // ETeleportType::ResetPhysics�� ���� ���¸� ������ �����մϴ�.
            // �ܼ� �̵��� ���Ѵٸ� ETeleportType::None�� �����մϴ�.
            PlayerCharacter->SetActorLocation(TargetWorldLocation, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}