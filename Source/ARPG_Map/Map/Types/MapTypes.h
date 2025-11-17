#pragma once

#include "CoreMinimal.h"
#include "MapTypes.generated.h"

UENUM(BlueprintType)
enum class EMapDirection : uint8
{
	None,
	North,
	East,
	South,
	West
};

// Represent a turn relative to a direction (e.g., right turn with North direction is the East Direction). Used for branches generation
UENUM(BlueprintType)
enum class EMapTurn : uint8
{
	None,
	Left,
	Right
};

// Function in the map (start, main path, end, wall, etc.)
UENUM(BlueprintType)
enum class EMapRole : uint8
{
	None,
	MainPathStart,
	MainPath,
	MainPathEnd,
	Wall
};

USTRUCT(BlueprintType)
struct FMapGraphCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Column = 0;

	// Default constructor needed by Unreal engine
	FMapGraphCoord() = default;	
	FMapGraphCoord(int32 InRow, int32 InColumn)
	: Row(InRow), Column(InColumn)
	{ }

	bool operator==(const FMapGraphCoord& Other) const { return Row == Other.Row && Column == Other.Column; }

	// Returns a new coordinate moved by Distance steps in the given direction (e.g., (0,0).Stepped(East, 2) returns (0,2))
	FMapGraphCoord Stepped(const EMapDirection Direction, const int32 Distance) const;

	// Sentinel value representing an invalid/uninitialized coordinate
	static const FMapGraphCoord None;
};