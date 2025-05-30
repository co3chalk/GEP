// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBase.h"
#include "Components/StaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h"

// Sets default values
APortalBase::APortalBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // ���� ��Ż�� Tick�� �ʿ� �����ϴ�.

    // Ʈ���� ���� ���� �� ����
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    if (TriggerVolume)
    {
        RootComponent = TriggerVolume; // Ʈ���� ������ ��Ʈ ������Ʈ�� ����
        TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // �ٸ� ���Ϳ� �������ǵ��� ����
        // (�ʿ��) TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f)); // �⺻ ũ�� ����
    }

    // (���� ����) ��Ż �޽� ������Ʈ ���� �� ����
    PortalMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
    if (PortalMeshComponent && TriggerVolume)
    {
        PortalMeshComponent->SetupAttachment(RootComponent); // ��Ʈ ������Ʈ(TriggerVolume)�� ���Դϴ�.
        PortalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �޽ô� �ݸ����� �ʿ� ���� �� �ֽ��ϴ�.
    }

    DestinationMapName = NAME_None; // �⺻�� �ʱ�ȭ
}

// Called when the game starts or when spawned
void APortalBase::BeginPlay()
{
    Super::BeginPlay();

    // ������ �̺�Ʈ�� �Լ� ���ε�
    if (TriggerVolume)
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalBase::OnPortalOverlap);
    }
}

void APortalBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // �������� ���Ͱ� ��ȿ�ϰ�, �÷��̾� ĳ�������� Ȯ�� (�ʿ�� ĳ����)
    // ����: AYourPlayerCharacter* PlayerCharacter = Cast<AYourPlayerCharacter>(OtherActor);
    // if (PlayerCharacter)
    if (OtherActor && (OtherActor != this)) // �ڱ� �ڽ��� �ƴ� �ٸ� ���Ϳ� �������Ǿ����� Ȯ��
    {
        if (DestinationMapName != NAME_None)
        {
            UE_LOG(LogTemp, Warning, TEXT("Overlapping with Portal. Attempting to open level: %s"), *DestinationMapName.ToString());
            UGameplayStatics::OpenLevel(this, DestinationMapName);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Portal overlap detected, but DestinationMapName is not set."));
        }
    }
}