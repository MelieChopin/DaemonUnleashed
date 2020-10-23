// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "KFGPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "KFGGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AKFGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:	
	AKFGGameMode();

	virtual void BeginPlay() override;

	//// PLAYER STAT
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	int maxLife = 0;
	UPROPERTY(BlueprintReadOnly)
	int currentLife = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	float transformRatio = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	float transformRatioSpeed = 0;
	////
	
};
