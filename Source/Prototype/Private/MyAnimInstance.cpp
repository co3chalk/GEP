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
		UE_LOG(LogTemp, Log, TEXT("fireMontage loaded successfully!")); // ���� �� �α� ���
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load fireMontage at path: AnimMontage'/Game/RadiantRobotRachel/Rachel/Animations/Anim_Rachel_BusterFire_Montage.Anim_Rachel_BusterFire_Montage'")); // ���� �� �α� ���
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