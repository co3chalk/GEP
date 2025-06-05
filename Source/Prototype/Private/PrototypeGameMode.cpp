// PrototypeGameMode.cpp
#include "PrototypeGameMode.h"
#include "PrototypeGameInstance.h"   // GameInstance ����� ���� (������Ʈ�� �� ������ �־�� ��)
#include "Kismet/GameplayStatics.h"
// ������ ����
APrototypeGameMode::APrototypeGameMode()
{
    // ���� �׷� ����� �⺻ ���¸� false�� �ʱ�ȭ�մϴ�.
    bPysGrabEnabledForCurrentLevel = false;

    // �ٸ� ��� �������� �⺻�� ������ �ʿ��ϴٸ� ���⼭ �մϴ�.
    // isClearedBasic = false; �� (UPROPERTY���� �̹� �ʱ�ȭ�ߴٸ� ���� ����)
}

// ���� ���� �� ȣ��Ǵ� �Լ�
void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay(); // �θ� Ŭ������ BeginPlay�� �׻� ���� ȣ���ϴ� ���� �����ϴ�.

    // GameInstance���� ���� �׷� ����� ���������� ��� �����Ǿ����� Ȯ���մϴ�.
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        if (GameInst->IsPysGrabFeatureUnlocked()) // GameInstance�� �Լ� ȣ��
        {
            SetPysGrabActiveForLevel(true); // GameMode �ڽ��� �Լ� ȣ��
        }
    }
    else
    {
        // GameInstance�� ã�� �� ���� ��� (��: ���� �� ���� ���� �� ��)
        UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode::BeginPlay - PrototypeGameInstance�� ã�� �� ���ų� Ÿ���� �ٸ��ϴ�. ���� �׷� ���´� �⺻��(��Ȱ��)�� �����մϴ�."));
    }
}
void APrototypeGameMode::IsClearedMap(bool isCleared)
{
	isCleared = true;

	UE_LOG(LogTemp, Warning, TEXT("plz."));
}

void APrototypeGameMode::IsGotKey(bool isGotKey)
{
	isGotKey = true;
}

// --- ���� �׷� ��� ���� �Լ� ���� ---
void APrototypeGameMode::SetPysGrabActiveForLevel(bool bEnable)
{
    bPysGrabEnabledForCurrentLevel = bEnable; // ��� ������ ���� �ùٸ��� ����

    if (bPysGrabEnabledForCurrentLevel)
    {
        UE_LOG(LogTemp, Log, TEXT("APrototypeGameMode: ���� ���� ���� �׷� ��� Ȱ��ȭ�� (bPysGrabEnabledForCurrentLevel = true)"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("APrototypeGameMode: ���� ���� ���� �׷� ��� ��Ȱ��ȭ�� (bPysGrabEnabledForCurrentLevel = false)"));
    }
}

bool APrototypeGameMode::IsPysGrabActiveForLevel() const
{
    return bPysGrabEnabledForCurrentLevel;
}