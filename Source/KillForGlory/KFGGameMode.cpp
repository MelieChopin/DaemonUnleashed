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

    TArray<AActor*> foundEnemy;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(),foundEnemy);

    enemyCount = foundEnemy.Num();
}

void AKFGGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(currentLife <= 0)
        myGameState = EKFGGameState::LOOSE;

    if(enemyCount <= 0)
        myGameState = EKFGGameState::WIN;
}
