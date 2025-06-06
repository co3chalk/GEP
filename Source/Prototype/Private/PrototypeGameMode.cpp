#include "PrototypeGameMode.h"
#include "PrototypeGameInstance.h"
#include "Kismet/GameplayStatics.h"


APrototypeGameMode::APrototypeGameMode()
{
    bPysGrabEnabledForCurrentLevel = false;
}

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();

    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        if (GameInst->IsPysGrabFeatureUnlocked())
        {
            SetPysGrabActiveForLevel(true);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode::BeginPlay - PrototypeGameInstance unfind."));
    }
}

// --- ������ ����(SET) �Լ��� ---

// MapIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
void APrototypeGameMode::IsClearedMap(int MapIndex, bool isCleared)
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        GameInst->SetClearedMap(MapIndex, isCleared);
    }
}

// KeyIndex: 0=Basic, 1=Flame, 2=Water, 3=Elec
void APrototypeGameMode::IsGotKey(int KeyIndex, bool isGotKey)
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        GameInst->SetGotKey(KeyIndex, isGotKey);
    }
}


// --- [�߰��� �κ�] ������ Ȯ��(GET) �Լ����� ���� ���� ---

bool APrototypeGameMode::IsMapCleared_Get(int MapIndex) const
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        // GameInstance�� ��� ��, �� ���(true/false)�� ��ȯ�մϴ�.
        return GameInst->GetClearedMapState(MapIndex);
    }
    // GameInstance�� ������ �����ϰ� false�� ��ȯ�մϴ�.
    return false;
}

bool APrototypeGameMode::HasGotKey_Get(int KeyIndex) const
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        // GameInstance�� ��� ��, �� ���(true/false)�� ��ȯ�մϴ�.
        return GameInst->GetGotKeyState(KeyIndex);
    }
    // GameInstance�� ������ �����ϰ� false�� ��ȯ�մϴ�.
    return false;
}


// --- ���� �׷� ��� ���� �Լ� (������ ����) ---

void APrototypeGameMode::SetPysGrabActiveForLevel(bool bEnable)
{
    bPysGrabEnabledForCurrentLevel = bEnable;

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