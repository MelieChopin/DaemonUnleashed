// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KFGPlayer.h"
#include "Engine/Engine.h"

AEnemyTank::AEnemyTank()
{
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
    
    if(isAttacking)
    {
        if(currentSpecialCooldown <= 0)
        {
            PlayAnimMontage(attackSpe);
            currentSpecialCooldown = specialCooldown;
        }
        else
            PlayAnimMontage(attackBasic);

        isAttacking = false;
    }
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
			/*StopAnimMontage(GetCurrentMontage());
			disableBasicAttack();
			FVector direction = GetActorLocation() - OtherActor->GetActorLocation();
			direction.Normalize();
			direction *= player->strengthPushBack;
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
			LaunchCharacter(FVector(direction.X, direction.Y,300),true,true);*/
    }
}

void AEnemyTank::startSpecialAttack()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AEnemyTank::stopSpecialAttack()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
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
