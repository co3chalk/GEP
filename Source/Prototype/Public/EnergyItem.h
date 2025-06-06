
#pragma once

#include "PrototypeGameMode.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyItem.generated.h"

class APrototypeGameMode;

UCLASS()
class PROTOTYPE_API AEnergyItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* sphereComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere)
	int32 id;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// 게임모드 받아오기
	UPROPERTY(EditAnywhere)
		APrototypeGameMode* gameMode = nullptr;

	UPROPERTY(EditAnywhere, Category = "EnergyItem")
		TArray<AActor*> PoweredTargets;
	void SetTargetsPowered(bool bOn);
};
