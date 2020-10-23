// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckIfPlayerIsNearToEnemy.h"

#include "Enemy.h"
#include "AIController.h"


bool UCheckIfPlayerIsNearToEnemy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if(enemy != nullptr)
        return enemy->isFollowingPlayer;
    
    return false;
}
