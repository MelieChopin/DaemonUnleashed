// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UBoxComponent;
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
    float distanceToFocusPlayer;

	UPROPERTY(EditAnywhere)
	float distanceWaitToAttackPlayer;
             
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isAttacking;

	UPROPERTY(EditAnywhere)
	bool isFollowingPlayer;

	float timeAttack;
	float changeIsAttacking;
	float timeAnimAttack = 0.6f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* attackBasic;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* attackSpe;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//// EnemyStat
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=EnemyStat)
	float currentLife = 0;
	////
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void EnemyDamage(int _damage);
	
};
