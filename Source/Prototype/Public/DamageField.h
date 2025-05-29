#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageField.generated.h" // ���� �̸��� ���� ����

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UDamageField : public UActorComponent // Ŭ���� �̸� ����
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDamageField(); // ������ �̸� ����

	// �� �ʵ尡 ���ϴ� �������� (������ �� �������Ʈ���� ���� ����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Settings")
	int32 DamageToDeal = 1;

	// �������� ������ ��� ���͸� �Է¹޴� �Լ� (�������Ʈ���� ȣ�� ����)
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplyDamageToActor(AActor* TargetActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};