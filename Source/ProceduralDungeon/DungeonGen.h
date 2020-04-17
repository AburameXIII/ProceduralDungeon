// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DungeonGen.generated.h"


const FVector2D CARDINAL[4] = { FVector2D(0,1),  FVector2D(0,-1), FVector2D(1,0), FVector2D(-1,0) };

UENUM()
enum ETileType {
	Empty, Wall, Water, Lava, Object, Enemy, RoomWall, Beginning, Exit
};

UENUM(BlueprintType)
enum class ERoomType : uint8 {
	Structured, Random
};

UENUM(BlueprintType)
enum class EPathType : uint8 {
	RightAndDown, Maze
};

USTRUCT()
struct FTile {
	GENERATED_BODY()
	ETileType Type;
	bool Room;
	int Region;
	FVector2D Coordinates;
};

USTRUCT()
struct FConnector {
	GENERATED_BODY()
    UPROPERTY()
	TArray<int> Regions;
	UPROPERTY()
	FVector2D Coordinates;
};


USTRUCT()
struct FRoom {
	GENERATED_BODY()
	UPROPERTY()
	int x;
	UPROPERTY()
	int y;
	UPROPERTY()
	int width;
	UPROPERTY()
	int height;
	UPROPERTY()
	int region;

	bool overlaps(FRoom OtherRoom, int padding) {
		bool OverlapX = (x >= OtherRoom.x - padding && x <= OtherRoom.x + OtherRoom.width + padding) ||
			(x + width >= OtherRoom.x - padding && x + width <= OtherRoom.x + OtherRoom.width + padding);
		bool OverlapY = (y >= OtherRoom.y - padding && y <= OtherRoom.y + OtherRoom.height + padding) ||
			(y + height >= OtherRoom.y - padding && y + height <= OtherRoom.y + OtherRoom.height + padding);
		return OverlapX && OverlapY;
	}

	bool operator==(const FRoom& OtherRoom) const {
		return x == OtherRoom.x && y == OtherRoom.y;
	}

};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALDUNGEON_API UDungeonGen : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDungeonGen();

	TArray<FTile> Generate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoomTries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExtraConnectorChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoomMinSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoomMaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoomPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WindingPercentage;

	UPROPERTY()
	TArray<FRoom> Rooms;

	UPROPERTY()
	TArray<FTile> DungeonArchitecture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HorizontalRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VerticalRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfRooms;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Deadends;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPathType PathType;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//Auxiliary Functions
	void AddRooms();
	bool Carve(FVector2D point, bool isRoom=false);
	void CarveRoomWall(FVector2D point);
	bool CanCarve(FVector2D point, FVector2D direction);
	void GrowMaze(int x, int y);
	void ConnectRegions();
	void ForceCarve(FVector2D p);
	void RemoveDeadEnds();

	void AddRoomsMD();
	void ConnectRegionsMD();
	bool CanConnectRight(FRoom r);
	bool CanConnectDown(FRoom r);
	void Connect(FRoom r1, FRoom r2);
	void ConnectTwoPoints(FVector2D p1, FVector2D p2);
	FRoom CreateMiniRoom(int r);
	void CarveRoom(FRoom r);
	FVector2D GetRandomRoomPoint(FRoom r);
	bool InBetween(int x, int y, int between);
	void Initialize();

	void Decorate();

	int CurrentRegion;
	FVector2D* LastDirection;
		
};
