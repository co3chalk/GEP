// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBase.h"
#include "Components/StaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h"

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
}

// Called when the game starts or when spawned
void APortalBase::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 이벤트에 함수 바인딩
    if (TriggerVolume)
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalBase::OnPortalOverlap);
    }
}

void APortalBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 오버랩된 액터가 유효하고, 플레이어 캐릭터인지 확인 (필요시 캐스팅)
    // 예시: AYourPlayerCharacter* PlayerCharacter = Cast<AYourPlayerCharacter>(OtherActor);
    // if (PlayerCharacter)
    if (OtherActor && (OtherActor != this)) // 자기 자신이 아닌 다른 액터와 오버랩되었는지 확인
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