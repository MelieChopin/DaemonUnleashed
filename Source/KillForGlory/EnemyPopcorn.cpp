// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPopcorn.h"



#include "KFGGameMode.h"
#include "Engine/Engine.h"

class AKFGPlayer;
// Sets default values
AEnemyPopcorn::AEnemyPopcorn()
{
	//AEnemy();
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isAttacking = false;
}

// Called when the game starts or when spawned
void AEnemyPopcorn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemyPopcorn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (isFollowingPlayer)
		timeAttack += DeltaTime;

	if (isAttacking)
	{
		if (changeIsAttacking <= 0.1)
			PlayAnimMontage(attackBasic);
		//GEngine->AddOnScreenDebugMessage(-5, 1.0f, FColor::Blue, FString::SanitizeFloat(timeAttack));
		changeIsAttacking += DeltaTime;
		if (changeIsAttacking >= timeAnimAttack)
		{
			StopAnimMontage(attackBasic);
			changeIsAttacking = 0.0f;
			isAttacking = false;
		}
	}

}

// Called to bind functionality to input
void AEnemyPopcorn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}






