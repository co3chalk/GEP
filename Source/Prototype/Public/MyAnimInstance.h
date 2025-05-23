// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * */
UCLASS()
class PROTOTYPE_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UMyAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	
	void JumpToSection(int32 SectionIndex);
	// UFUNCTION ��ũ�� �߰� (�������Ʈ���� ȣ�� �����ϵ���)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayfireMontage(float InPlayRate = 1.0f); // �Ű����� �߰� (MyAnimInstance.cpp�� ������ ��ġ)

	// UFUNCTION ��ũ�� �߰� (�������Ʈ���� ȣ�� �����ϵ���)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FName GetfireMontageName(int32 SectionIndex);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsFalling;
	// �� �κ��� �����Ǿ����ϴ�. 'class' Ű���� �߰� �� Category ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	class UAnimMontage* fireMontage; // 'class' Ű���带 �߰��Ͽ� ���� �����մϴ�.


};