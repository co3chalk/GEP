#include "DamageField.h" // ��� ���� �̸� ����
#include "PrototypeCharacter.h" // �������� �� PrototypeCharacter Ŭ������ �˱� ���� ����

// Sets default values for this component's properties
UDamageField::UDamageField() // ������ �̸� ����
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UDamageField::BeginPlay()
{
	Super::BeginPlay();
}

// �������� �����ϴ� �Լ� ����
void UDamageField::ApplyDamageToActor(AActor* TargetActor)
{
	// TargetActor�� ��ȿ����, �׸��� �� ������Ʈ�� ���� ���� �ڽ��� �ƴ��� Ȯ��
	if (!TargetActor || TargetActor == GetOwner())
	{
		return;
	}

	// TargetActor�� APrototypeCharacter�� ĳ���� �õ�
	APrototypeCharacter* Character = Cast<APrototypeCharacter>(TargetActor);

	// ĳ���� ���� �� (��, �������� ���Ͱ� �츮 �÷��̾� ĳ������ ���)
	if (Character)
	{
		// ĳ���Ϳ��� ������ ����
		UE_LOG(LogTemp, Log, TEXT("DamageField: Applying %d damage to %s"), DamageToDeal, *Character->GetName());
		Character->TakeDamage(DamageToDeal);
	}
}