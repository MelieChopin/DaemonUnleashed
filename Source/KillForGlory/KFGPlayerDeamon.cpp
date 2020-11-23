// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerDeamon.h"

#include "Enemy.h"
#include "KFGPlayerHuman.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

AKFGPlayerDeamon::AKFGPlayerDeamon()
{
	attackHitBox = CreateDefaultSubobject<UBoxComponent>("AttackHitBox");
    attackHitBox->SetupAttachment(RootComponent);
	attackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKFGPlayerDeamon::OnAttackHitBoxBeginOverlap);

}


void AKFGPlayerDeamon::BeginPlay()
{
	Super::BeginPlay();

	attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	isPossessed = false;
}

void AKFGPlayerDeamon::SetHumanForm(AKFGPlayerHuman* _humanForm)
{
    if(_humanForm != nullptr)
    {
	    humanForm = _humanForm;
    	humanForm->DisableAttackHitBox();
    }
}

void AKFGPlayerDeamon::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	
	if(isPossessed)
	{
		humanForm->SetActorTransform(GetActorTransform());

		if(*deamonResources > 0)
			*deamonResources -= DeltaTime * *deamonResourcesSpeedDecrease;

		if(*deamonResources <= 0)
		{
			TransformToHuman();
			return;
		}
	}

	if (playerState == EPlayerState::PARADE)
	{
		timeCurrentParade += DeltaTime;
		if (timeCurrentParade >= timeParade)
		{
			StopAnimMontage(parade);
			playerState = EPlayerState::NONE;
			isUntouchable = false;
		}
	}

	if(currentCooldownSpecialAttack > 0 && playerState != EPlayerState::SPECIAL)
		currentCooldownSpecialAttack -= DeltaTime;

}

void AKFGPlayerDeamon::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Transform", IE_Pressed, this, &AKFGPlayerDeamon::TransformToHuman);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Parade", IE_Pressed,this, &AKFGPlayerDeamon::BeginParade);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AKFGPlayerDeamon::Attack);
	PlayerInputComponent->BindAction("Special", IE_Pressed, this, &AKFGPlayerDeamon::Special);
    //Call parent
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AKFGPlayerDeamon::BeginParade()
{
	if (playerState == EPlayerState::ROLL && playerState == EPlayerState::PARADE && playerState == EPlayerState::SPECIAL)
		return;

	if (playerState == EPlayerState::ATTACK)
		StopAnimMontage(GetCurrentMontage());

	timeCurrentParade = 0.0f;
	PlayAnimMontage(parade);
	isUntouchable = true;
}

void AKFGPlayerDeamon::MoveForward(float Value)
{
	// Cancel attack anim if player is in recover time
	if ( (Controller != NULL) && (Value != 0.0f))
	{
		if(recoverAttack)
		{
			StopAnimMontage(GetCurrentMontage());
			AttackReset();
		}
		Super::MoveForward(Value);
	}
}

void AKFGPlayerDeamon::MoveRight(float Value)
{
	// Cancel attack anim if player is in recover time
	if ( (Controller != NULL) && (Value != 0.0f))
	{
		if(recoverAttack)
		{
			StopAnimMontage(GetCurrentMontage());
			AttackReset();
		}
		Super::MoveRight(Value);
	}
}



void AKFGPlayerDeamon::Attack()
{
	if(timeStun > 0)
		return;
	
	bufferAttack = true;
	if(playerState != EPlayerState::ATTACK && playerState != EPlayerState::ROLL)
	{
		attackNum = 0;
		changePlayerState(EPlayerState::ATTACK);
		AttackLaunch();
	}
	else if(playerState == EPlayerState::ATTACK && recoverAttack)
		AttackLaunch();	
}

void AKFGPlayerDeamon::AttackLaunch()
{
	if(!bufferAttack)
		return;

	AActor* enemyTarget = findNearestEnemyFromInput();
    
	if(enemyTarget != nullptr)
	{
		FRotator rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),enemyTarget->GetActorLocation());
		rot.Pitch = 0;
		rot.Roll = 0;
		SetActorRotation(rot);
	}
	
	UAnimMontage* animToPlay = nullptr;

	bufferAttack = false;
	recoverAttack = false;
	
	switch (attackNum)
	{
	case 0:
		animToPlay = combo1Anim;
		break;
	case 1:
		animToPlay = combo2Anim;
		break;
	default:
		break;
	}

	if(animToPlay != nullptr)
		PlayAnimMontage(animToPlay);

	attackNum = attackNum < 1 ? attackNum+1 : 0;
}

void AKFGPlayerDeamon::AttackReset()
{
	playerState = EPlayerState::NONE;

	bufferAttack = false;
	recoverAttack = false;
	attackNum = 0;
}

void AKFGPlayerDeamon::EnableAttackHitBox()
{
	attackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AKFGPlayerDeamon::DisableAttackHitBox()
{
	attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKFGPlayerDeamon::OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-3, 1, FColor::Blue, "HIT DEAMON");
	if(OtherActor->ActorHasTag("Enemy"))
	{
		AEnemy* enemy = Cast<AEnemy>(OtherActor);
		if(enemy != nullptr)
			enemy->EnemyDamage(attackDamage);
		*currentLife += lifeSteal;
	}
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(cameraShakeBasic, 1.0f);
}

void AKFGPlayerDeamon::Special()
{
	if(currentCooldownSpecialAttack > 0 || timeStun > 0)
		return;

	currentCooldownSpecialAttack = cooldownSpecialAttack;
	
	TArray<AActor*> foundEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(),foundEnemy);
    
	for(AActor* actorEnemy : foundEnemy)
	{
		if(FVector::Dist(actorEnemy->GetActorLocation(),GetActorLocation()) <= specialRange)
		{
			AEnemy* enemy = Cast<AEnemy>(actorEnemy);
			if(enemy == nullptr)
				continue;
			GEngine->AddOnScreenDebugMessage(-1,1,FColor::Blue,"test");
			FVector launchDir = GetActorLocation() - enemy->GetActorLocation();
			launchDir *= 2;
			enemy->LaunchCharacter(FVector(launchDir.X,launchDir.Y,350),true,true);
			enemy->EnemyDamage(specialDamage, true);
		}
	}
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(cameraShakeSpecial, 1.0f);
}

AActor* AKFGPlayerDeamon::findNearestEnemyFromInput()
{
	TArray<AActor*> foundEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(),foundEnemy);
    
	AActor* enemyTarget = nullptr;
	for(AActor* enemy : foundEnemy)
	{
		if(FVector::Dist(enemy->GetActorLocation(),GetActorLocation()) <= 350.f && !Cast<AEnemy>(enemy)->isDead)
		{
			if(enemyTarget == nullptr)
			{
				enemyTarget = enemy;
				continue;
			}
            
			FVector inputDir(GetInputAxisValue("MoveForward"),GetInputAxisValue("MoveRight"),0);
			inputDir.Normalize();

			if (Controller != nullptr)
			{
				FRotator Rotation = Controller->GetControlRotation();
				FRotator YawRotation(0, Rotation.Yaw, 0);

				FVector Direction = GetActorLocation() + (YawRotation.Quaternion() * inputDir) *150;
            
				if(FVector::Dist(enemy->GetActorLocation(),Direction) < FVector::Dist(enemyTarget->GetActorLocation(),Direction))
					enemyTarget = enemy;
			}
		}
	}

	return enemyTarget;
}

void AKFGPlayerDeamon::changePlayerState(EPlayerState newPlayerState)
{
	if(playerState == newPlayerState)
		return;
	
	switch (playerState)
	{
	case EPlayerState::ATTACK:
		AttackReset(); break;
	case EPlayerState::SPECIAL: break;
	case EPlayerState::NONE: break;
	default: ;
	}

	playerState = newPlayerState;
}


void AKFGPlayerDeamon::TransformToHuman()
{
	if(timeStun > 0)
		return;
	
    if(humanForm != nullptr)
    {
        humanForm->SetActorTransform(GetActorTransform());
        humanForm->SetActorEnableCollision(true);
        humanForm->SetActorHiddenInGame(false);
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        APlayerCameraManager* cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
        FRotator camRotation = cameraManager->GetCameraRotation();
        FRotator playerRotation = GetActorRotation();
        GetController()->Possess(humanForm);
        humanForm->GetCharacterMovement()->Velocity = GetVelocity();
        GetWorld()->GetFirstPlayerController()->RotationInput = camRotation-playerRotation;
    	isPossessed = false;
    	humanForm->isPossessed = true;
    	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(cameraShakeTransformation, 1.0f);
    }
}


