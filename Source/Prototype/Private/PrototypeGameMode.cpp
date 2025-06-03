// PrototypeGameMode.cpp
#include "PrototypeGameMode.h"
#include "PlayerUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PrototypeCharacter.h"

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode::BeginPlay() - Called"));

    if (PlayerHealthUIClass != nullptr) //
    {
        UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerHealthUIClass is VALID")); //
        PlayerHealthUIInstance = CreateWidget<UPlayerUIWidget>(GetWorld(), PlayerHealthUIClass); //
        if (PlayerHealthUIInstance != nullptr) //
        {
            UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerHealthUIInstance CREATED and VALID, adding to viewport.")); //
            //PlayerHealthUIInstance->AddToViewport(); //

            APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)); //
            if (PlayerCharacter) //
            {
                UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerCharacter FOUND, initializing UI for player.")); //
                PlayerHealthUIInstance->InitializeForPlayer(PlayerCharacter); //
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("APrototypeGameMode: PlayerCharacter NOT FOUND. Initializing UI with null player.")); //
                PlayerHealthUIInstance->InitializeForPlayer(nullptr); //
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("APrototypeGameMode: PlayerHealthUIInstance FAILED TO CREATE.")); //
        }
    }
    else // if (PlayerHealthUIClass != nullptr) ø° ¥Î«— else
    {
        UE_LOG(LogTemp, Error, TEXT("APrototypeGameMode: PlayerHealthUIClass is NULL. Check GameMode Blueprint defaults.")); //
    }
}