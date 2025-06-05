// PrototypeGameMode.cpp
#include "PrototypeGameMode.h"
#include "PrototypeGameInstance.h"   // GameInstance 사용을 위해 (프로젝트에 이 파일이 있어야 함)
#include "Kismet/GameplayStatics.h"
// 생성자 구현
APrototypeGameMode::APrototypeGameMode()
{
    // 물리 그랩 기능의 기본 상태를 false로 초기화합니다.
    bPysGrabEnabledForCurrentLevel = false;

    // 다른 멤버 변수들의 기본값 설정이 필요하다면 여기서 합니다.
    // isClearedBasic = false; 등 (UPROPERTY에서 이미 초기화했다면 생략 가능)
}

// 레벨 시작 시 호출되는 함수
void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay를 항상 먼저 호출하는 것이 좋습니다.

    // GameInstance에서 물리 그랩 기능이 영구적으로 잠금 해제되었는지 확인합니다.
    UPrototypeGameInstance* GameInst = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInst)
    {
        if (GameInst->IsPysGrabFeatureUnlocked()) // GameInstance의 함수 호출
        {
            SetPysGrabActiveForLevel(true); // GameMode 자신의 함수 호출
        }
    }
    else
    {
        // GameInstance를 찾을 수 없는 경우 (예: 개발 중 아직 설정 안 함)
        UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode::BeginPlay - PrototypeGameInstance를 찾을 수 없거나 타입이 다릅니다. 물리 그랩 상태는 기본값(비활성)을 유지합니다."));
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

// --- 물리 그랩 기능 관련 함수 구현 ---
void APrototypeGameMode::SetPysGrabActiveForLevel(bool bEnable)
{
    bPysGrabEnabledForCurrentLevel = bEnable; // 멤버 변수의 값을 올바르게 변경

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