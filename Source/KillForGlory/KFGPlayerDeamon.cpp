// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerDeamon.h"
#include "KFGPlayerHuman.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"

void AKFGPlayerDeamon::SetHumanForm(AKFGPlayerHuman* _humanForm)
{
    if(_humanForm != nullptr)
        humanForm = _humanForm;
}

void AKFGPlayerDeamon::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Transform", IE_Pressed, this, &AKFGPlayerDeamon::TransformToHuman);
    //Call parent
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AKFGPlayerDeamon::TransformToHuman()
{
    if(humanForm != nullptr)
        GetController()->Possess(humanForm);
}
