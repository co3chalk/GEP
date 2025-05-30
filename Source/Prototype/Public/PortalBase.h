// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PortalBase.generated.h"

UCLASS()
class PROTOTYPE_API APortalBase : public AActor
{
	GENERATED_BODY()
	
public:
    // Sets default values for this actor's properties
    APortalBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // 포탈의 트리거 볼륨입니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UBoxComponent* TriggerVolume;

    // (선택 사항) 포탈의 시각적 표현을 위한 스태틱 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalMeshComponent;

    // 이동할 목표 맵의 이름입니다. 블루프린트 및 인스턴스에서 편집 가능합니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Settings")
    FName DestinationMapName;

    // 플레이어가 포탈에 오버랩되었을 때 호출될 함수입니다.
    UFUNCTION()
    void OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};