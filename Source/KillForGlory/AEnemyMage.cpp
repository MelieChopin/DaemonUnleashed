// Fill out your copyright notice in the Description page of Project Settings.


#include "AEnemyMage.h"

#include "Particle.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AAEnemyMage::AAEnemyMage()
{
   
}

void AAEnemyMage::BeginPlay()
{
    Super::BeginPlay();
    cooldown = cooldownInvocation;
}

void AAEnemyMage::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (isTouching)
    {
        StopAnimMontage(GetCurrentMontage());
        changeIsAttacking = 0.0f;
        timeInvocation = 0.0f;
        if (listInvocation.Num() != 0)
            while (listInvocation.Num() != 0)
                listInvocation.RemoveAt(0);
        return;
    }
    
    if (isFollowingPlayer)
    {
        timeAttack += DeltaTime;
        cooldown -= DeltaTime;
    }

    if (cooldown <= 0.0f && numberOfUses > 0 && changeIsAttacking != 0.0f && !isDead)
    {
        if (timeInvocation <= 0.1)
            PlayAnimMontage(attackSpe);

        timeInvocation += DeltaTime;
        if (timeInvocation >= 3.0f)
        {
            StopAnimMontage(attackSpe);
            timeInvocation = 0.0f;
            cooldown = cooldownInvocation;
            numberOfUses -= 1;
            changeIsAttacking = 0.0f;
            timeAttack = 0.0f;
            isAttacking = false;
        }
        return;
    }

    if (isAttacking && !isDead)
    {
        if (changeIsAttacking <= 0.1)
            PlayAnimMontage(attackBasic);

        changeIsAttacking += DeltaTime;
        if (changeIsAttacking >= timeAnimAttack)
        {
            StopAnimMontage(attackBasic);
            changeIsAttacking = 0.0f;
            isAttacking = false;
            changeIsAttacking = 0.0f;
        }
    }
}


void AAEnemyMage::CreateMagicBall()
{
    AActor* temp = GetWorld()->SpawnActor<AActor>(magicBall.Get(), GetActorLocation() + GetActorForwardVector() * 200, FRotator::ZeroRotator);
    AParticle* particle = Cast<AParticle>(temp);
    if (particle != nullptr)
    {
        FVector newDirection = GetActorForwardVector();
        FVector zDirection = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
        zDirection.Normalize();
        newDirection.Z = zDirection.Z;
        particle->direction = newDirection;
        particle->damage = damage;
    }
}

void AAEnemyMage::CreateInvocation()
{
    for (int i = 0; i < numberOfInvocation; i++)
    {
        FVector newLocation(FMath::RandRange(GetActorLocation().X + GetActorForwardVector().X * 200, GetActorLocation().X + GetActorForwardVector().X * 600),
                FMath::RandRange(GetActorLocation().Y + GetActorForwardVector().Y * 200, GetActorLocation().Y + GetActorForwardVector().Y * 600),
                GetActorLocation().Z - 90);
        listInvocation.Add(GetWorld()->SpawnActor<AActor>(invocation.Get(), newLocation, FRotator::ZeroRotator));      
    }
}

void AAEnemyMage::ActiveInvocation()
{
    if (listInvocation.Num() == 0)
        return;

    for (int i = 0; i < numberOfInvocation; i++)
    {
        AParticle* invocation1 = Cast<AParticle>(listInvocation[0]);
        if (invocation1 != nullptr)
        {
            GEngine->AddOnScreenDebugMessage(-2, 1.0, FColor::Black, TEXT("ACTIF"));
            invocation1->isActived = true;
            listInvocation.RemoveAt(0);
        }
    }
}