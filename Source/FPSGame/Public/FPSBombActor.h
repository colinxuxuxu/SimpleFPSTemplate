// Fill out your copyright notice in the Description page of Project Settings.
// Copyright Epic Games, Inc. All Rights Reserved. Tutorial by Tom Looman

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBombActor.generated.h"

class UStaticMeshComponent;
class UParticleSystem;

UCLASS()
class FPSGAME_API AFPSBombActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFPSBombActor();

protected:

	//Notes:visible anywhere allows changes to the variable, but not the class
	//components need to be instantiated in constructor
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UMaterialInstanceDynamic* MaterialInstance;
	FLinearColor CurrentColor;
	FLinearColor TargetColor;


	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		float ExplodeDelay;
	UFUNCTION()
		void Explode();

	//need to assign in UE editor
	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		UParticleSystem* ExplosionTemplate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
