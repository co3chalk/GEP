// PortalTeleporterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalTeleporterBase.generated.h" // 실제 생성된 헤더 이름으로 변경

// 전방 선언
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROTOTYPE_API APortalTeleporterBase : public AActor // 
{
    GENERATED_BODY()

public:
    // 생성자
    APortalTeleporterBase();

protected:
    // 게임 시작 또는 스폰 시 호출
    virtual void BeginPlay() override;

public:
    // 플레이어 감지를 위한 트리거 볼륨
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components")
    UBoxComponent* TriggerVolume;

    // (선택 사항) 포탈의 시각적 표현을 위한 스태틱 메시 컴포넌트
    // 블루프린트 클래스 디폴트에서 메시를 변경할 수 있도록 EditDefaultsOnly 추가
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Portal Components")
    UStaticMeshComponent* PortalVisualMesh;

    // 이동할 목표 월드 좌표
    // 인스턴스별로 에디터에서 수정 가능하도록 EditAnywhere 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport Settings", meta = (MakeEditWidget = true))
    FVector TargetWorldLocation; // 월드 좌표임을 명시

    // 오버랩 발생 시 호출될 함수
    UFUNCTION()
    void OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};