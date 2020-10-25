// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Engine/Blueprint.h"

#include "AEnemyMage.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AAEnemyMage : public AEnemy
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere)
	UBlueprint* enemyClass;


protected:


	
};
