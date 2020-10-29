// Fill out your copyright notice in the Description page of Project Settings.


#include "IsAttacking.h"

#include "AIController.h"
#include "Enemy.h"

bool UIsAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AEnemy* enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if (enemy != nullptr)
        if (enemy->isAttacking)
            return true;

    return false;
}
