// PortalTeleporterBase.cpp

#include "PortalTeleporterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h" // 플레이어 캐릭터 캐스팅을 위해 (또는 사용하는 특정 캐릭터 클래스)
// #include "Kismet/GameplayStatics.h" // 필요시 사용

APortalTeleporterBase::APortalTeleporterBase()
{
    PrimaryActorTick.bCanEverTick = false; // Tick이 필요 없다면 꺼둡니다.

    // 루트 컴포넌트 역할을 할 트리거 볼륨 생성
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    if (TriggerVolume)
    {
        RootComponent = TriggerVolume;
        TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 동적 액터와 오버랩되도록 설정
        // TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f)); // 필요시 기본 크기 설정
    }

    // 포탈 시각 메시 컴포넌트 생성 (선택 사항)
    PortalVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalVisualMesh"));
    if (PortalVisualMesh && RootComponent)
    {
        PortalVisualMesh->SetupAttachment(RootComponent); // 루트 컴포넌트에 부착
        PortalVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 시각용이므로 콜리전 없음
    }

    // 목표 위치 기본값 설정
    TargetWorldLocation = FVector::ZeroVector;
}

void APortalTeleporterBase::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 이벤트에 함수 바인딩
    if (TriggerVolume)
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortalTeleporterBase::OnPortalOverlap);
    }
}

void APortalTeleporterBase::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 자기 자신이나 null 액터는 무시
    if (OtherActor && (OtherActor != this))
    {
        // 플레이어 캐릭터인지 확인 (또는 원하는 다른 액터 타입으로 캐스팅)
        ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor); // 모든 캐릭터 타입 허용
        // 또는 특정 플레이어 캐릭터로 제한하고 싶다면:
        // AYourPlayerCharacter* PlayerCharacter = Cast<AYourPlayerCharacter>(OtherActor);

        if (PlayerCharacter)
        {
            UE_LOG(LogTemp, Log, TEXT("Player '%s' overlapped with portal. Teleporting to %s."), *PlayerCharacter->GetName(), *TargetWorldLocation.ToString());

            // 지정된 TargetWorldLocation으로 액터 위치 설정
            // ETeleportType::TeleportPhysics는 이동 시 물리 상태를 초기화하여 안정성을 높입니다.
            // ETeleportType::ResetPhysics는 물리 상태를 완전히 리셋합니다.
            // 단순 이동만 원한다면 ETeleportType::None도 가능합니다.
            PlayerCharacter->SetActorLocation(TargetWorldLocation, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}