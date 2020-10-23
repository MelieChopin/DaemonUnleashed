// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "CheckIfPlayerIsNearToEnemy.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API UCheckIfPlayerIsNearToEnemy : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};
