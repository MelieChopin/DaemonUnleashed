// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KFGPlayer.h"
#include "Engine/Engine.h"

AEnemyTank::AEnemyTank()
{
    PrimaryActorTick.bCanEverTick = true;
    
    attackHitBox = CreateDefaultSubobject<UBoxComponent>("basicAttackHitBox");
    attackHitBox->SetupAttachment(RootComponent);
    
    specialHitBox = CreateDefaultSubobject<UBoxComponent>("specialAttackHitBox");
    specialHitBox->SetupAttachment(RootComponent);
}

void AEnemyTank::BeginPlay()
{
    Super::BeginPlay();
    
    attackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyTank::OnAttackHitBoxBeginOverlap);
    specialHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyTank::OnSpecialHitBoxBeginOverlap);

    attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyTank::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (isFollowingPlayer)
    {
        timeAttack += DeltaTime;
        currentSpecialCooldown -= DeltaTime;
    }

    if (isTouching)
    {
        ResetPercentMat();
        disableSpecialAttack();
        disableBasicAttack();
    }
    
    if(isAttacking && changeIsAttacking <= 0 && !isDead)
    {
        if(currentSpecialCooldown <= 0)
        {
            PlayAnimMontage(attackSpe);
            currentSpecialCooldown = specialCooldown;
        }
        else
            PlayAnimMontage(attackBasic);
    }
    
    if(isAttacking)
    {
        changeIsAttacking += DeltaTime;
        if (changeIsAttacking >= timeAnimAttack)
        {
            changeIsAttacking = 0.0f;
            isAttacking = false;
        }
    }

    if(!isAttacking && GetCharacterMovement()->GetMovementName() == "Flying")
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

}

void AEnemyTank::ColorYellow()
{
	GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 1.0f);
    GetMesh()->SetScalarParameterValueOnMaterials("Yellow", 0.3f);
}

void AEnemyTank::ResetPercentMat()
{
    GetMesh()->SetScalarParameterValueOnMaterials("PercentForBasicColor", 1.0f);
    GetMesh()->SetScalarParameterValueOnMaterials("Yellow", 0.0f);
}

void AEnemyTank::enableBasicAttack()
{
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AEnemyTank::disableBasicAttack()
{
    attackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    isAttacking = false;
}


void AEnemyTank::OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Basic tank");
    if(OtherActor->ActorHasTag("Player") && OtherComp->ComponentHasTag("Body"))
    {
        AKFGPlayer* player = Cast<AKFGPlayer>(OtherActor);
        if(player != nullptr)

		if (!player->isUntouchable)
		{
		    player->PlayerDamage(damage);
		    return;
		}
    }
}

void AEnemyTank::startSpecialAttack()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AEnemyTank::stopSpecialAttack()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
    isAttacking = false;
}

void AEnemyTank::enableSpecialAttack()
{
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AEnemyTank::disableSpecialAttack()
{
    specialHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);    
}

void AEnemyTank::OnSpecialHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor->ActorHasTag("Player") && OtherComp->ComponentHasTag("Body"))
    {
        GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Special tank");
        
        AKFGPlayer* player = Cast<AKFGPlayer>(OtherActor);
        if(player != nullptr)
        {
            player->PlayerDamage(specialDamage);
            player->PlayerStun(specialStun);
        }
    }
}
