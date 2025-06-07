// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBase.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PrototypeGameMode.h"

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

	PortalEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffectComponent"));
	if (PortalEffectComponent)
	{
		// ��Ż �޽ð� �ִٸ� �װ���, ���ٸ� ��Ʈ�� ���Դϴ�.
		// �Ǵ� ��Ż ȿ���� �߽��� �� Ư�� SceneComponent�� ���� ���� ���� �ֽ��ϴ�.
		if (PortalMeshComponent)
		{
			PortalEffectComponent->SetupAttachment(PortalMeshComponent);
		}
		else
		{
			PortalEffectComponent->SetupAttachment(RootComponent);
		}
		PortalEffectComponent->SetAutoActivate(false); // �⺻�����δ� �ڵ� Ȱ��ȭ���� ����
	}

	PortalEffectAsset = nullptr; // �⺻���� ����
	bAutoActivatePortalEffectOnBeginPlay = true; // �⺻�����δ� BeginPlay �� �ڵ� Ȱ��ȭ

}

// Called when the game starts or when spawned
void APortalBase::BeginPlay()
{
	Super::BeginPlay();

	// --- ����: ���̾ư��� ȿ�� ���� �� Ȱ��ȭ ���� �߰� ---
	if (PortalEffectComponent && PortalEffectAsset) // ������Ʈ�� ������ ��� ��ȿ���� Ȯ��
	{
		PortalEffectComponent->SetAsset(PortalEffectAsset); // �������Ʈ���� ������ ������ ������Ʈ�� �Ҵ�

		if (bAutoActivatePortalEffectOnBeginPlay) // �ڵ� Ȱ��ȭ �ɼ��� ���� �ִٸ�
		{
			PortalEffectComponent->Activate(true); // ���̾ư��� ȿ�� Ȱ��ȭ (true�� �̹� Ȱ��ȭ�� ��� �����)
			UE_LOG(LogTemp, Log, TEXT("PortalBase: Niagara effect activated on BeginPlay."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("PortalBase: Niagara effect asset set, but bAutoActivatePortalEffectOnBeginPlay is false."));
		}
	}
	else if (!PortalEffectAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("PortalBase: PortalEffectAsset is not set. Niagara effect cannot be played."));
	}
	// --- ��: ���̾ư��� ȿ�� ���� �� Ȱ��ȭ ���� �߰� ---

	// ������ �̺�Ʈ�� �Լ� ���ε� (�� �κ��� ������ �����ϰ� ����)
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalBase::OnPortalOverlap);
	}
}

void APortalBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this)) // �ڱ� �ڽ��� �ƴ� �ٸ� ���Ϳ� �������Ǿ����� Ȯ��
	{
		FString NowMapName = GetWorld()->GetMapName();
		NowMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		// [������ �κ�] ���ο� ������ �����ϴ� ��� ���� ��� ������ �Ҵ��մϴ�.
		gameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(this));

		if ((DestinationMapName != NAME_None) && gameMode)
		{
			if (NowMapName == TEXT("WireMap"))
			{
				gameMode->IsClearedMap(0, true);
				UE_LOG(LogTemp, Warning, TEXT("Wire Cleared."));
			}
			else if (NowMapName == TEXT("FlameMap"))
			{
				gameMode->IsClearedMap(1, true);
				UE_LOG(LogTemp, Warning, TEXT("Flame Cleared."));
			}
			else if (NowMapName == TEXT("WaterMap"))
			{
				gameMode->IsClearedMap(2, true);
				UE_LOG(LogTemp, Warning, TEXT("Water Cleared."));
			}
			else if (NowMapName == TEXT("ElecMap"))
			{
				gameMode->IsClearedMap(3, true);
				UE_LOG(LogTemp, Warning, TEXT("Elec Cleared."));
			}

			// PortalEnter();
			UE_LOG(LogTemp, Warning, TEXT("Overlapping with Portal. Attempting to open level: %s"), *DestinationMapName.ToString());
			
			PortalEnter(); // �������Ʈ���� ������ �̺�Ʈ ȣ��
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Portal overlap detected, but DestinationMapName is not set."));
		}
	}
}