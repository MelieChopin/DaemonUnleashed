// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFGPlayer.h"
#include "KFGPlayerDeamon.generated.h"

class UBoxComponent;

class USphereComponent;
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

    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaTime) override;
protected:
    class AKFGPlayerHuman* humanForm = nullptr;

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface

    /** Called for forwards/backward input */
    void MoveForward(float Value) override;

    /** Called for side to side input */
    void MoveRight(float Value) override;

public:
    //// ANIM
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
    UAnimMontage* combo1Anim;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
    UAnimMontage* combo2Anim;
    ///
    
    //// ATTACK
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
    UBoxComponent* attackHitBox;

    UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable)
    void AttackLaunch();
    UFUNCTION(BlueprintCallable)
    void AttackReset();

    UFUNCTION(BlueprintCallable)
    void EnableAttackHitBox();
    UFUNCTION(BlueprintCallable)
    void DisableAttackHitBox();
    
    void Attack();
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

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Charge)
    bool isCharging;
    
    FVector forwardVector;
    FVector beginLocation;
    float currentDistance = 0.0f;
    float yawRate;

    //UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Collision)
    //UBoxComponent* collisionCharge;

    void BeginCharge();
    void Charge();
    UFUNCTION(BlueprintCallable)
    void EndCharge();
    ////

    // Basic attack
    bool bufferAttack = false;
    UPROPERTY(BlueprintReadWrite)
    bool recoverAttack = false;
    int attackNum = 0;

    // Special Attack
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Special)
    float specialRange = 0;
    void Special();
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Special)
    float cooldownSpecialAttack = 0;
    float currentCooldownSpecialAttack = 0;
    
    //// PlayerStat
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
    int attackDamage = 0;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
    int specialDamage = 0;
    ////

    bool isPossessed;

    AActor* findNearestEnemyFromInput();
    void changePlayerState(EPlayerState newPlayerState);
    
    void TransformToHuman();
};
