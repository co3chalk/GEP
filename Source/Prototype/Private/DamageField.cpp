#include "DamageField.h" // 헤더 파일 이름 수정
#include "PrototypeCharacter.h" // 데미지를 줄 PrototypeCharacter 클래스를 알기 위해 포함

// Sets default values for this component's properties
UDamageField::UDamageField() // 생성자 이름 수정
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UDamageField::BeginPlay()
{
	Super::BeginPlay();
}

// 데미지를 적용하는 함수 구현
void UDamageField::ApplyDamageToActor(AActor* TargetActor)
{
	// TargetActor가 유효한지, 그리고 이 컴포넌트를 가진 액터 자신이 아닌지 확인
	if (!TargetActor || TargetActor == GetOwner())
	{
		return;
	}

	// TargetActor를 APrototypeCharacter로 캐스팅 시도
	APrototypeCharacter* Character = Cast<APrototypeCharacter>(TargetActor);

	// 캐스팅 성공 시 (즉, 오버랩된 액터가 우리 플레이어 캐릭터일 경우)
	if (Character)
	{
		// 캐릭터에게 데미지 적용
		UE_LOG(LogTemp, Log, TEXT("DamageField: Applying %d damage to %s"), DamageToDeal, *Character->GetName());
		Character->TakeDamage(DamageToDeal);
	}
}