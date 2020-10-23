// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerDeamon.h"

#include <xkeycheck.h>


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

	yawRate = GetCharacterMovement()->RotationRate.Yaw;
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
        humanForm = _humanForm;
}

void AKFGPlayerDeamon::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	
	if(isPossessed)
	{
		humanForm->SetActorTransform(GetActorTransform());
	}

	if (playerState == EPlayerState::ROLL)
		Charge();

	if(currentCooldownSpecialAttack > 0 && playerState != EPlayerState::SPECIAL)
		currentCooldownSpecialAttack -= DeltaTime;

}

void AKFGPlayerDeamon::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Transform", IE_Pressed, this, &AKFGPlayerDeamon::TransformToHuman);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Roll_Charge", IE_Pressed, this, &AKFGPlayerDeamon::BeginCharge);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AKFGPlayerDeamon::Attack);
	PlayerInputComponent->BindAction("Special", IE_Pressed, this, &AKFGPlayerDeamon::Special);
    //Call parent
    Super::SetupPlayerInputComponent(PlayerInputComponent);
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

void AKFGPlayerDeamon::BeginCharge()
{
	if (playerState == EPlayerState::ROLL || GetMovementComponent()->IsFalling() || (playerState == EPlayerState::ATTACK && !recoverAttack))
		return;

	if(recoverAttack)
		StopAnimMontage(GetCurrentMontage());
	changePlayerState(EPlayerState::ROLL);
	
	beginLocation = GetActorLocation();

	forwardVector = GetActorForwardVector() * 200;
	GetCharacterMovement()->Velocity = forwardVector;

	currentDistance = 0.0f;
	GetCharacterMovement()->RotationRate = FRotator(0, trajectoryAdjustment, 0);
	isCharging = true;
    GEngine->AddOnScreenDebugMessage(-3, 1.0f, FColor::Red, TEXT("BEGINCHARGE"));
}

void AKFGPlayerDeamon::Charge()
{
	GetCharacterMovement()->Velocity = GetVelocity() + FMath::Lerp(forwardVector, GetActorForwardVector() * 200, lerpPercent);
	forwardVector = FMath::Lerp(forwardVector, GetActorForwardVector() * 200, lerpPercent);

	currentDistance += (beginLocation - GetActorLocation()).Size();
	beginLocation = GetActorLocation();
	if (currentDistance >= distanceMax || GetMovementComponent()->IsFalling())
		EndCharge();
}

void AKFGPlayerDeamon::EndCharge()
{
	GetCharacterMovement()->RotationRate = FRotator(0, yawRate, 0);
	playerState = EPlayerState::NONE;
	isCharging = false;
	GEngine->AddOnScreenDebugMessage(-3, 1.0f, FColor::Red, TEXT("ENDCHARGE"));
}

void AKFGPlayerDeamon::Attack()
{
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
	GEngine->AddOnScreenDebugMessage(-3, 1, FColor::Blue, "HIT");
	if(OtherActor->ActorHasTag("Enemy"))
	{
		AEnemy* enemy = Cast<AEnemy>(OtherActor);
		if(enemy != nullptr)
			enemy->EnemyDamage(attackDamage);
	}
}

void AKFGPlayerDeamon::Special()
{
	if(currentCooldownSpecialAttack > 0)
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
			enemy->EnemyDamage(specialDamage);
		}
	}
}

AActor* AKFGPlayerDeamon::findNearestEnemyFromInput()
{
	TArray<AActor*> foundEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(),foundEnemy);
    
	AActor* enemyTarget = nullptr;
	for(AActor* enemy : foundEnemy)
	{
		if(FVector::Dist(enemy->GetActorLocation(),GetActorLocation()) <= 350.f)
		{
			if(enemyTarget == nullptr)
			{
				enemyTarget = enemy;
				continue;
			}
            
			FVector inputDir(GetInputAxisValue("MoveForward"),GetInputAxisValue("MoveRight"),0);
			inputDir.Normalize();
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			FVector Direction = GetActorLocation() + (YawRotation.Quaternion() * inputDir) *150;
            
			if(FVector::Dist(enemy->GetActorLocation(),Direction) < FVector::Dist(enemyTarget->GetActorLocation(),Direction))
				enemyTarget = enemy;
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
	case EPlayerState::ROLL:
        EndCharge(); break;
	case EPlayerState::SPECIAL: break;
	case EPlayerState::NONE: break;
	default: ;
	}

	playerState = newPlayerState;
}


void AKFGPlayerDeamon::TransformToHuman()
{
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
    }
}


