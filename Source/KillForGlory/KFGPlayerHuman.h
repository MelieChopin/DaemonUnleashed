// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFGPlayer.h"
#include "KFGPlayerHuman.generated.h"

/**
 * 
 */
UCLASS()
class KILLFORGLORY_API AKFGPlayerHuman : public AKFGPlayer
{
	GENERATED_BODY()

public:
	AKFGPlayerHuman();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category= Forme)
	TSubclassOf<class AKFGPlayerDeamon> deamonFormClass = nullptr;
protected:
	class AKFGPlayerDeamon* deamonForm = nullptr;
	
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* combo3Anim;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* rollAnim;
	////

	//// Collider
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* attackHitBox;
	////
	
	//// Event
	void Roll(); //Input Roll Event
	
	UFUNCTION(BlueprintCallable)
	void RollStart();
	UFUNCTION(BlueprintCallable)
    void RollEnd();

	UFUNCTION(BlueprintCallable)
    void AttackLaunch();
	UFUNCTION(BlueprintCallable)
    void AttackReset();
	
	UFUNCTION(BlueprintCallable)
    void EnableAttackHitBox();
	UFUNCTION(BlueprintCallable)
    void DisableAttackHitBox();

	void Attack(); //Input Attack Event
	void TransformToDeamon();
	
	UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	////
	
	bool isRolling = false;
	bool isAttacking = false;
	bool bufferAttack = false;
	UPROPERTY(BlueprintReadWrite)
	bool recoverAttack = false;
	int attackNum = 0;
	
	void changePlayerState(EPlayerState newPlayerState);
};
