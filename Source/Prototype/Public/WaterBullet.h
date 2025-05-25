// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterBullet.generated.h"

UCLASS()
class PROTOTYPE_API AWaterBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 원기둥 형태 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	// 투사체 움직임 컴포넌트 (기본 이동용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	// 투사체 생존 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "WaterBullet")
	float LifeSpan = 3.0f;

};
