// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFGPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

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
	virtual void Tick(float DeltaTime) override;
	
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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Anim)
	UAnimMontage* specialAttack;
	////

	//// Collider
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* attackHitBox;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	UBoxComponent* specialHitBox;
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
	void EnableSpecial();
	UFUNCTION(BlueprintCallable)
    void DisableSpecial();
	UFUNCTION(BlueprintCallable)
    void SpecialEnd();
	
	UFUNCTION(BlueprintCallable)
    void EnableAttackHitBox();
	UFUNCTION(BlueprintCallable)
    void DisableAttackHitBox();

	void Attack(); //Input Attack Event
	void Special();
	void TransformToDeamon();
	void Jumping();
	
	UFUNCTION()
    void OnAttackHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	////

	//// WALLJUMp
	bool isFreeze = false;
	float timeForWallJump = 0.0f;
	FVector normal;
	UPROPERTY(EditAnywhere, Category=WallJump)
	float timeBeforeFalling;
	UPROPERTY(EditAnywhere, Category=WallJump)
	float speedFriction;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Collision)
	USphereComponent* wallJumpSphereCollision;

	UFUNCTION()
    void OnWallJumpBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnWallJumpEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                            class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	////

	//// PlayerStat
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	int attackDamage = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=PlayerStat)
	int specialDamage = 0;
	////
	
	bool bufferAttack = false;
	UPROPERTY(BlueprintReadWrite)
	bool recoverAttack = false;
	int attackNum = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Special)
	float cooldownSpecialAttack = 0;
	float currentCooldownSpecialAttack = 0;
	void SpecialActorOverlapped(); // Called every frame during special attack
	
	void changePlayerState(EPlayerState newPlayerState);
};
