// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGGameMode.h"

#include "Enemy.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AKFGGameMode::AKFGGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Actor/PlayerHuman_Blueprint"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}


void AKFGGameMode::BeginPlay()
{
    Super::BeginPlay();
    currentLife = maxLife;

    TArray<AActor*> winZoneSearch;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(),"WinZone",winZoneSearch);

    if(winZoneSearch.Num() <= 0)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(),nullptr,EQuitPreference::Quit,true);
        return;
    }

    winZone = winZoneSearch[0];
}

void AKFGGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(winZone == nullptr)
        return;
    
    TArray<AActor*> enemyList;
    winZone->GetOverlappingActors(enemyList,AEnemy::StaticClass());
    
    if(currentLife <= 0)
        myGameState = EKFGGameState::LOOSE;

    if(enemyList.Num() <= 0)
        myGameState = EKFGGameState::WIN;

    //GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,FString::FromInt(enemyList.Num()));

    for(auto test : enemyList)
        GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,test->GetName());
}
