// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "KFGGameMode.generated.h"

UENUM(Blueprintable)
enum class EKFGGameState : uint8
{
	RUN,
	PAUSE,
	WIN,
	LOOSE,
};

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
	virtual void Tick(float DeltaTime) override;

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
	
	//// Game managment
	UPROPERTY(BlueprintReadWrite)
	EKFGGameState myGameState = EKFGGameState::RUN;
	UPROPERTY(BlueprintReadWrite)
	int enemyCount = 0;
	
};
