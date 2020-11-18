// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackPlayer.h"

#include "AIController.h"
#include "Enemy.h"
#include "Engine/Engine.h"

EBTNodeResult::Type UAttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if(enemy == nullptr)
        return EBTNodeResult::Aborted;
    
    if (enemy->timeAttack > enemy->speedAttack)
    {
        enemy->timeAttack = 0.0f;
        enemy->isAttacking = true;
        enemy->changeIsAttacking = 0.0f;
    }
      
    return EBTNodeResult::Succeeded;
}    