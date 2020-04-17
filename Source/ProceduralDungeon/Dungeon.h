// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGen.h"
#include "Block.h"
#include "Dungeon.generated.h"

UCLASS()
class PROCEDURALDUNGEON_API ADungeon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDungeonGen* DunGen;

	UPROPERTY(VisibleAnywhere)
	TArray<ABlock*> DungeonArchitecture;

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void Generate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//Generates and shows the dungeon
	void ShowDungeon();

};
