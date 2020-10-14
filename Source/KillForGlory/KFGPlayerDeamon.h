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
    AKFGPlayerDeamon();

    void SetHumanForm(class AKFGPlayerHuman* _humanForm);

    virtual void Tick(float DeltaTime) override;
protected:
    class AKFGPlayerHuman* humanForm = nullptr;

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface

    //// ATTACK
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
    UBoxComponent* attackHitBox;

    UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void Attack();

    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
    
    ///
    

    //// CHARGE
    UPROPERTY(EditAnywhere, Category=Charge)
    float distanceMax;
    UPROPERTY(EditAnywhere, Category=Charge)
    float speedMovement;
    UPROPERTY(EditAnywhere, Category=Charge)
    float trajectoryAdjustment;
    UPROPERTY(EditAnywhere, Category=Charge)
    float lerpPercent;

    FVector forwardVector;
    FVector beginLocation;
    float currentDistance = 0.0f;
    float yawRate;

    void BeginCharge();
    void Charge();
    void EndCharge();
    ////
    
   
    
    void TransformToHuman();
};
