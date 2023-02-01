// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GamePlayTestCharacter.generated.h"


UCLASS(config=Game)
class AGamePlayTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Interact;

	UPROPERTY(EditAnywhere, Category = Jump, meta = (AllowPrivateAccess = "true"))
	int MaxJumps = 0;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

	UPROPERTY(EditAnywhere, Category = "Components")
    class UPhysicsHandleComponent* PhysicsHandle;
	

public:
	AGamePlayTestCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	
protected:
	
	bool bIsGrabbing;
	bool bIsFlying;
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	//Interaction 
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BeginInteract();

	//Grab and Throw
	UFUNCTION(BlueprintCallable)
	UPrimitiveComponent* GrabObject(bool bUseActorLocation, bool bDebug, float Range);
	UFUNCTION(BlueprintCallable)
	void ThrowObject(UPrimitiveComponent* ComponentToGrab, float Amount);
	UFUNCTION(BlueprintCallable)
	void GrabbingLoop(bool bUseActorLocation,float Range);

	// Flying
	UFUNCTION(BlueprintCallable)
	void ToggleFlying(EMovementMode Mode, bool isFlying	);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UseMasterFeild(FHitResult hit);
};

