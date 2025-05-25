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

	// ����� ���� �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	// ����ü ������ ������Ʈ (�⺻ �̵���)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	// ����ü ���� �ð� (��)
	UPROPERTY(EditDefaultsOnly, Category = "WaterBullet")
	float LifeSpan = 3.0f;

};
