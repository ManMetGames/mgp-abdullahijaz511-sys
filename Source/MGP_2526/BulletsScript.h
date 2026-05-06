// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletsScript.generated.h"

UCLASS()
class MGP_2526_API ABulletsScript : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletsScript();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float TimeMult= 1.0f;
	float PrevTimeMult= 1.0f;
	float scale;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
