#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/MapGraphUtils.h"
#include "ARPG_Map/Map/Types/MapTypes.h"
#include "MapLayout.generated.h"

// =============================================
// Layout structs (Path is an array of segments)
// =============================================

// Represents a straight line of cells in the map graph with a start coordinate, direction, length. It has a type defining what it represents
// (Corridor, river, wall, etc.) and a theme defining visuals.
USTRUCT(BlueprintType)
struct FMapSegment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapGraphCoord Start = FMapGraphCoord::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Length = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection Direction = EMapDirection::None;
	
	// what it represents (Corridor, river, wall, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapTileType Type = EMapTileType::None;

	// Define visuals
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme = "None";
	
	// Get the coordinate of the call at the segment index. Index represent the cell of the segment
	FORCEINLINE FMapGraphCoord GetCoordAt(const int32 Index) const { return Start.Stepped(Direction, Index); }
	
	FORCEINLINE FMapGraphCoord End() const { return Start.Stepped(Direction, Length - 1); }
	
	FORCEINLINE bool IsValid() const { return Length > 0 && Direction != EMapDirection::None; };
};

// =======================================
// Layout generation configuration structs
// =======================================

// Shape of the Path
UENUM(BlueprintType)
enum class EMapPathLayout : uint8
{
	None,
	Straight,
	L,
	U,	
	Stairs,
	Square
};

// Configuration to generate a path
USTRUCT(BlueprintType)
struct FMapPathConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapPathLayout Layout = EMapPathLayout::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SegmentMinLength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SegmentMaxLength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapTileType Type = EMapTileType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme;

	FORCEINLINE bool IsValid() const
	{
		return Layout != EMapPathLayout::None && SegmentMinLength > 0 && SegmentMaxLength >= SegmentMinLength;
	}
};

// Configuration to generate a branch from a path
USTRUCT(BlueprintType)
struct FMapBranchConfig
{
	GENERATED_BODY()

	// RotationFromMainPath relative to the main path direction
	UPROPERTY(EditAnywhere)
	EMapRotation RotationFromMainPath = EMapRotation::None;

	// Interval between two branches
	UPROPERTY(EditAnywhere)
	int32 StepInterval = 0;

	// Percent probability of spawn from 0 to 1
	UPROPERTY(EditAnywhere)
	float SpawnProbability = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapPathConfig PathConfig;
};

// Configurations to generate a layout.
USTRUCT(BlueprintType)
struct FMapLayoutConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapPathConfig MainPathConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapBranchConfig> BranchConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 0.f;

	FORCEINLINE bool IsValid() const
	{
		return MainPathConfig.IsValid();
	}
};

// Constraint for generating the path.
USTRUCT(BlueprintType)
struct FMapPathConstraints
{
	GENERATED_BODY()

	// Start call of the first segment
	FMapGraphCoord Start;

	// Direction of the first segment
	EMapDirection StartDirection;

	// Bounds of the path. Should be set with the rows and columns of the graph
	FMapGraphCoord Bounds = FMapGraphCoord(0, 0);

	bool IsInBounds(const FMapGraphCoord& Coord) const
	{
		return Coord.Row >= 0 && Coord.Row <= Bounds.Row && Coord.Column >= 0 && Coord.Column <= Bounds.Column;
	}
};