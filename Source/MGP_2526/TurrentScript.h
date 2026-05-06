// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurrentScript.generated.h"

UCLASS()
class MGP_2526_API ATurrentScript : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurrentScript();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Fire(); 

	FTimerHandle FireTimerHandle; // for ther cooldown between firings


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AActor> BulletClass; // the class for the bullet that the turrent will fire. set in bp for demo. In actual game this would be an enemy ai choosing which "attack" to fire

	// initial speed of the bullet
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BulletSpeed = 2000.0f; 


	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireInterval = 5.0f; 

};
