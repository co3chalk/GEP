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
	PrimaryActorTick.bCanEverTick = false; // 보통 포탈은 Tick이 필요 없습니다.

	// 트리거 볼륨 생성 및 설정
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume)
	{
		RootComponent = TriggerVolume; // 트리거 볼륨을 루트 컴포넌트로 설정
		TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 다른 액터와 오버랩되도록 설정
		// (필요시) TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f)); // 기본 크기 설정
	}

	// (선택 사항) 포탈 메시 컴포넌트 생성 및 설정
	PortalMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	if (PortalMeshComponent && TriggerVolume)
	{
		PortalMeshComponent->SetupAttachment(RootComponent); // 루트 컴포넌트(TriggerVolume)에 붙입니다.
		PortalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시는 콜리전이 필요 없을 수 있습니다.
	}

	DestinationMapName = NAME_None; // 기본값 초기화

	PortalEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffectComponent"));
	if (PortalEffectComponent)
	{
		// 포탈 메시가 있다면 그곳에, 없다면 루트에 붙입니다.
		// 또는 포탈 효과의 중심이 될 특정 SceneComponent를 만들어서 붙일 수도 있습니다.
		if (PortalMeshComponent)
		{
			PortalEffectComponent->SetupAttachment(PortalMeshComponent);
		}
		else
		{
			PortalEffectComponent->SetupAttachment(RootComponent);
		}
		PortalEffectComponent->SetAutoActivate(false); // 기본적으로는 자동 활성화하지 않음
	}

	PortalEffectAsset = nullptr; // 기본값은 없음
	bAutoActivatePortalEffectOnBeginPlay = true; // 기본적으로는 BeginPlay 시 자동 활성화

}

// Called when the game starts or when spawned
void APortalBase::BeginPlay()
{
	Super::BeginPlay();

	// --- 시작: 나이아가라 효과 설정 및 활성화 로직 추가 ---
	if (PortalEffectComponent && PortalEffectAsset) // 컴포넌트와 에셋이 모두 유효한지 확인
	{
		PortalEffectComponent->SetAsset(PortalEffectAsset); // 블루프린트에서 설정한 에셋을 컴포넌트에 할당

		if (bAutoActivatePortalEffectOnBeginPlay) // 자동 활성화 옵션이 켜져 있다면
		{
			PortalEffectComponent->Activate(true); // 나이아가라 효과 활성화 (true는 이미 활성화된 경우 재시작)
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
	// --- 끝: 나이아가라 효과 설정 및 활성화 로직 추가 ---

	// 오버랩 이벤트에 함수 바인딩 (이 부분은 기존과 동일하게 유지)
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalBase::OnPortalOverlap);
	}
}

void APortalBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this)) // 자기 자신이 아닌 다른 액터와 오버랩되었는지 확인
	{
		FString NowMapName = GetWorld()->GetMapName();
		NowMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		// [수정된 부분] 새로운 변수를 선언하는 대신 기존 멤버 변수에 할당합니다.
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
			
			PortalEnter(); // 블루프린트에서 구현된 이벤트 호출
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Portal overlap detected, but DestinationMapName is not set."));
		}
	}
}