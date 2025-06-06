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

// --- 데이터 설정(SET) 함수들 ---

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


// --- [추가된 부분] 데이터 확인(GET) 함수들의 실제 구현 ---

bool APrototypeGameMode::IsMapCleared_Get(int MapIndex) const
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        // GameInstance에 물어본 후, 그 결과(true/false)를 반환합니다.
        return GameInst->GetClearedMapState(MapIndex);
    }
    // GameInstance가 없으면 안전하게 false를 반환합니다.
    return false;
}

bool APrototypeGameMode::HasGotKey_Get(int KeyIndex) const
{
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        // GameInstance에 물어본 후, 그 결과(true/false)를 반환합니다.
        return GameInst->GetGotKeyState(KeyIndex);
    }
    // GameInstance가 없으면 안전하게 false를 반환합니다.
    return false;
}


// --- 물리 그랩 기능 관련 함수 (기존과 동일) ---

void APrototypeGameMode::SetPysGrabActiveForLevel(bool bEnable)
{
    bPysGrabEnabledForCurrentLevel = bEnable;

    if (bPysGrabEnabledForCurrentLevel)
    {
        UE_LOG(LogTemp, Log, TEXT("APrototypeGameMode: 현재 레벨 물리 그랩 기능 활성화됨 (bPysGrabEnabledForCurrentLevel = true)"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("APrototypeGameMode: 현재 레벨 물리 그랩 기능 비활성화됨 (bPysGrabEnabledForCurrentLevel = false)"));
    }
}

bool APrototypeGameMode::IsPysGrabActiveForLevel() const
{
    return bPysGrabEnabledForCurrentLevel;
}