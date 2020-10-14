// Fill out your copyright notice in the Description page of Project Settings.


#include "KFGPlayerDeamon.h"
#include "KFGPlayerHuman.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

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
    {
        humanForm->SetActorEnableCollision(true);
        humanForm->SetActorHiddenInGame(false);
        humanForm->SetActorTransform(GetActorTransform());
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        APlayerCameraManager* cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
        FRotator camRotation = cameraManager->GetCameraRotation();
        FRotator playerRotation = GetActorRotation();
        GetController()->Possess(humanForm);
        humanForm->GetCharacterMovement()->Velocity = GetVelocity();
        GetWorld()->GetFirstPlayerController()->RotationInput = camRotation-playerRotation;
    }
}
