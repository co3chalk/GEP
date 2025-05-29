#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageField.generated.h" // 파일 이름에 맞춰 수정

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UDamageField : public UActorComponent // 클래스 이름 수정
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDamageField(); // 생성자 이름 수정

	// 이 필드가 가하는 데미지량 (에디터 및 블루프린트에서 설정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
	int32 DamageToDeal = 1;

	// 데미지를 적용할 대상 액터를 입력받는 함수 (블루프린트에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplyDamageToActor(AActor* TargetActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};