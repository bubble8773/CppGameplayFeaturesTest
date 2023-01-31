// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"IInteractWithObjects.h"
#include "Interacbale_Base.generated.h"

UCLASS()
class GAMEPLAYTEST_API AInteracbale_Base : public AActor, public IIInteractWithObjects
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteracbale_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void Interact() override;*/


};
