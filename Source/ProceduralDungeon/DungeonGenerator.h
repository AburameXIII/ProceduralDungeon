// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dungeon.h"

/**
 * 
 */


class PROCEDURALDUNGEON_API DungeonGenerator
{
public:
	DungeonGenerator();
	~DungeonGenerator();

	

	int RoomTries;
	float ExtraConnectorChance;
	int RoomMinSize;
	int RoomMaxSize;
	int Width;
	int Height;

	int WindingPercentage;

protected:
	

	
	//Regions
	//CurrentRegion

private:
	void AddRooms();
	void GrowMaze(int x, int y);
	bool CanCarve(FVector2D point, FVector2D direction);
	void Carve(FVector2D point);
	void ConnectRegions();

	int CurrentRegion;
	//ADungeon* Dungeon;
	FVector2D* LastDirection;


};
