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
    // ��Ż�� Ʈ���� �����Դϴ�.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UBoxComponent* TriggerVolume;

    // (���� ����) ��Ż�� �ð��� ǥ���� ���� ����ƽ �޽� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalMeshComponent;

    // �̵��� ��ǥ ���� �̸��Դϴ�. �������Ʈ �� �ν��Ͻ����� ���� �����մϴ�.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Settings")
    FName DestinationMapName;

    // �÷��̾ ��Ż�� �������Ǿ��� �� ȣ��� �Լ��Դϴ�.
    UFUNCTION()
    void OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};