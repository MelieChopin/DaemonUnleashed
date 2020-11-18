// Fill out your copyright notice in the Description page of Project Settings.


#include "CanAttackPlayer.h"

#include "AIController.h"
#include "Enemy.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

bool UCanAttackPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if(enemy != nullptr)
    {
        FVector enemyLocation = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
        FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
        
        if (FVector::Dist(enemyLocation,playerLocation) > enemy->distanceWaitToAttackPlayer + 60.f)
            return false;
    }
   
    return true;
}
