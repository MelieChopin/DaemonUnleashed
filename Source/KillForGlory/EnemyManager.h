// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EnemyPopcorn.h"
#include "GameFramework/Character.h"
#include "EnemyManager.generated.h"

UCLASS()
class KILLFORGLORY_API AEnemyManager : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyManager();

	UPROPERTY(EditAnywhere)
	int numberOfEnemies;
	
	UPROPERTY(EditAnywhere)
	float sizeSpawn;
	
	UPROPERTY()
	TArray<AActor*> listEnemies;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyPopcorn> enemyClass = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};