// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFGPlayer.h"
#include "KFGPlayerDeamon.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AKFGPlayerDeamon : public AKFGPlayer
{
	GENERATED_BODY()
public:
    void SetHumanForm(class AKFGPlayerHuman* _humanForm);
protected:
    class AKFGPlayerHuman* humanForm = nullptr;

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface
    
    void TransformToHuman();
};
