// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class KILLFORGLORY_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere)
    float speedAttack;
             
    UPROPERTY(EditAnywhere)
    float damage;
             
    UPROPERTY(EditAnywhere)
    float speedMove;
             
    UPROPERTY(EditAnywhere)
    float distanceToFocusPlayer;
             
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isAttacking;

	UPROPERTY(EditAnywhere)
	bool isFollowingPlayer;

	float timeAttack;
	float changeIsAttacking;
	float timeAnimAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
