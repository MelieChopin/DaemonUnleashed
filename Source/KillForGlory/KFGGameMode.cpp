// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGGameMode.h"

#include "UObject/ConstructorHelpers.h"

AKFGGameMode::AKFGGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Actor/Player_Blueprint"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
