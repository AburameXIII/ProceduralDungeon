// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class PROCEDURALDUNGEON_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

	//Mesh of the tile
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Auxiliary functions to set the different materials
	void SetRoomWall();
	void SetBeginning();
	void SetExit();
	void SetItem();
	void SetEnemy();

private:
	//Different possible materials for tiles
	class UMaterial* RoomWallMaterial;
	class UMaterial* BeginningMaterial;
	class UMaterial* ExitMaterial;
	class UMaterial* EnemyMaterial;
	class UMaterial* ItemMaterial;


};
