// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"


UMyAnimInstance::UMyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/RadiantRobotRachel/Rachel/Animations/Anim_Rachel_BusterFire_Montage.Anim_Rachel_BusterFire_Montage'"));
	if (AM.Succeeded())
	{
		fireMontage = AM.Object;
		UE_LOG(LogTemp, Log, TEXT("fireMontage loaded successfully!")); // 성공 시 로그 출력
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load fireMontage at path: AnimMontage'/Game/RadiantRobotRachel/Rachel/Animations/Anim_Rachel_BusterFire_Montage.Anim_Rachel_BusterFire_Montage'")); // 실패 시 로그 출력
	}
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
			IsFalling = Character->GetMovementComponent()->IsFalling();
		}
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