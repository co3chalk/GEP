// PrototypeGameMode.cpp
#include "PrototypeGameMode.h"
#include "PlayerUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PrototypeCharacter.h"


void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode::BeginPlay() - Called")); // 로그 추가

    if (PlayerHealthUIClass != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerHealthUIClass is VALID")); // 로그 추가
        PlayerHealthUIInstance = CreateWidget<UPlayerUIWidget>(GetWorld(), PlayerHealthUIClass);
        if (PlayerHealthUIInstance != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerHealthUIInstance CREATED and VALID, adding to viewport.")); // 로그 추가
            PlayerHealthUIInstance->AddToViewport();

            APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
            if (PlayerCharacter)
            {
                UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerCharacter FOUND, initializing UI for player.")); // 로그 추가
                PlayerHealthUIInstance->InitializeForPlayer(PlayerCharacter);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerCharacter NOT FOUND. Initializing UI with null player.")); // 로그 추가
                PlayerHealthUIInstance->InitializeForPlayer(nullptr);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("APrototypeGameMode: PlayerHealthUIInstance FAILED TO CREATE.")); // 오류 로그 추가
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("APrototypeGameMode: PlayerHealthUIClass is NULL. Check GameMode Blueprint defaults.")); // 오류 로그 추가
    }
}