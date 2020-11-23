// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


#include "KFGGameMode.h"
#include "KFGPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

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

	if (isTouching)
	{
		if (currentTimeEnemyRed <= 0.1f)
			PlayAnimMontage(getHit);
		currentTimeEnemyRed += DeltaTime;
		if (currentTimeEnemyRed >= timeEnemyRed)
		{
			StopAnimMontage(getHit);
			GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 1.0f);
			GetMesh()->SetScalarParameterValueOnMaterials("PercentForRed", 0.0f);
			currentTimeEnemyRed = 0.0f;
			isTouching = false;
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::EnemyDamage(int _damage, bool attackSpecial)
{
	if (currentLife > 0)
	{
		currentLife -= _damage;
		GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 0.5f);
        GetMesh()->SetScalarParameterValueOnMaterials("PercentForRed", 0.9f);
		isTouching = true;
		if (currentLife > 0 && !attackSpecial)
		{
         	currentTimeEnemyRed = 0.0f;
			FVector launchDir = GetActorLocation() - GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
            launchDir.Normalize();
            launchDir *= strengthPushBack;
			GetCharacterMovement()->Velocity = - FVector::UpVector * 10;
            LaunchCharacter(FVector(launchDir.X, launchDir.Y,300),true,true);
		}
	}
		
	if (currentLife <= 0 && !isDead)
	{
		isDead = true;
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	}
	
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,FString::FromInt(currentLife));
}


