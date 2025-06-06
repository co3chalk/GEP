// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PrototypeCharacter.h" // APrototypeCharacter 클래스의 정보를 가져오기 위해 include합니다.



UMyAnimInstance::UMyAnimInstance()
{
	fireMontage = nullptr;
}



void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsFalling = Character->GetMovementComponent()->IsFalling();\
		}
		
	}
	if (!IsValid(OwnerCharacter))
	{
		
		OwnerCharacter = Cast<APrototypeCharacter>(TryGetPawnOwner());
	}

	// OwnerCharacter가 유효한지(null이 아닌지) 다시 한번 확실하게 확인합니다.
	if (IsValid(OwnerCharacter))
	{
		
		HP = OwnerCharacter->GetCurrentHP(); // 이제 이 코드는 안전하게 실행됩니다.
	}
}

// PlayfireMontage 함수 구현 (h 파일의 선언과 이름/매개변수가 일치해야 합니다)
void UMyAnimInstance::PlayfireMontage(float InPlayRate /*= 1.0f*/) { // 매개변수 추가 (h 파일 선언과 일치)
	if (fireMontage)
	{
		// 몽타주가 현재 재생 중이 아니라면 재생합니다. (중복 재생 방지)
		if (!Montage_IsPlaying(fireMontage))
		{
			Montage_Play(fireMontage, InPlayRate);
			UE_LOG(LogTemp, Log, TEXT("fireMontage played!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("fireMontage is already playing."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("fireMontage is null, cannot play!"));
	}
}
void UMyAnimInstance::JumpToSection(int32 SectionIndex) {}
FName UMyAnimInstance::GetfireMontageName(int32 SectionIndex)
{
	return FName(*FString::Printf(TEXT("fire%d"), SectionIndex));
}