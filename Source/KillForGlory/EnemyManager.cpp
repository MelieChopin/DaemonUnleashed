// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"


// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	int xMin, xMax, yMin, yMax;
	xMin = GetActorLocation().X - sizeSpawn;
	xMax = xMin + 2 * sizeSpawn;
	yMin = GetActorLocation().Y - sizeSpawn;
	yMax = yMin + 2 * sizeSpawn;
	
	for (int i = 0; i < numberOfEnemies; i++)
	{
		FVector newPosition(FMath::RandRange(xMin, xMax), FMath::RandRange(yMin, yMax), GetActorLocation().Z);
		GEngine->AddOnScreenDebugMessage(-3, 1.0f, FColor::Black, newPosition.ToString());
		GetWorld()->SpawnActor<AActor>(enemyClass.Get(), GetActorLocation(), FRotator::ZeroRotator);
	}	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


