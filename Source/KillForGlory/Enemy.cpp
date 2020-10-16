// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Engine/Engine.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::EnemyDamage(int _damage)
{
	if(currentLife > 0)
		currentLife -= _damage;
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,FString::FromInt(currentLife));
}
