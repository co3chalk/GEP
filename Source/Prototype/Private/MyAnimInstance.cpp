// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PrototypeCharacter.h" // APrototypeCharacter Ŭ������ ������ �������� ���� include�մϴ�.



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

	// OwnerCharacter�� ��ȿ����(null�� �ƴ���) �ٽ� �ѹ� Ȯ���ϰ� Ȯ���մϴ�.
	if (IsValid(OwnerCharacter))
	{
		
		HP = OwnerCharacter->GetCurrentHP(); // ���� �� �ڵ�� �����ϰ� ����˴ϴ�.
	}
}

// PlayfireMontage �Լ� ���� (h ������ ����� �̸�/�Ű������� ��ġ�ؾ� �մϴ�)
void UMyAnimInstance::PlayfireMontage(float InPlayRate /*= 1.0f*/) { // �Ű����� �߰� (h ���� ����� ��ġ)
	if (fireMontage)
	{
		// ��Ÿ�ְ� ���� ��� ���� �ƴ϶�� ����մϴ�. (�ߺ� ��� ����)
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