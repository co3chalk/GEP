// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PrototypeGameMode.h"
#include "PortalBase.generated.h"
class UStaticMeshComponent; // 이미 전방 선언되어 있을 수 있지만, 명시적으로 추가
class UNiagaraComponent;    // 나이아가라 컴포넌트를 위한 전방 선언
class UNiagaraSystem;       // 나이아가라 시스템 에셋을 위한 전방 선언

UCLASS()
class PROTOTYPE_API APortalBase : public AActor
{
	GENERATED_BODY()
	
public:
    // Sets default values for this actor's properties
    APortalBase();
    // --- 시작: 나이아가라 효과 관련 프로퍼티 ---

  // 블루프린트에서 이 포탈에 사용할 나이아가라 시스템 에셋을 지정합니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal FX", meta = (DisplayName = "Portal Effect Asset"))
    UNiagaraSystem* PortalEffectAsset;

    // 지정된 에셋으로 나이아가라 효과를 재생할 컴포넌트입니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal FX", meta = (DisplayName = "Portal Effect Component"))
    UNiagaraComponent* PortalEffectComponent;

    // (선택 사항) 게임 시작 시 포탈 효과를 자동으로 활성화할지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal FX")
    bool bAutoActivatePortalEffectOnBeginPlay;

    // --- 끝: 나이아가라 효과 관련 프로퍼티 ---

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

    // 게임모드 받아오기
    UPROPERTY(EditAnywhere)
    APrototypeGameMode* gameMode = nullptr;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
        void PortalEnter();// 구현은 블루프린트
};