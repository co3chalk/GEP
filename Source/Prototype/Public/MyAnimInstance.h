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
	// UFUNCTION 매크로 추가 (블루프린트에서 호출 가능하도록)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayfireMontage(float InPlayRate = 1.0f); // 매개변수 추가 (MyAnimInstance.cpp의 구현과 일치)

	// UFUNCTION 매크로 추가 (블루프린트에서 호출 가능하도록)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FName GetfireMontageName(int32 SectionIndex);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsFalling;
	// 이 부분이 수정되었습니다. 'class' 키워드 추가 및 Category 변경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	class UAnimMontage* fireMontage; // 'class' 키워드를 추가하여 전방 선언합니다.


};