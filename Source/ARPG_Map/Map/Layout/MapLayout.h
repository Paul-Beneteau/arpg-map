#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/MapGraphUtils.h"
#include "ARPG_Map/Map/Types/MapTypes.h"
#include "MapLayout.generated.h"

// Represent main path shape of the map
UENUM(BlueprintType)
enum class EMapLayout : uint8
{
	None,
	Straight,
	L,
	Z,
	S,
	U,
	Cross,
	Ring,
	Fork
};

// Part of a segment which have a theme (visual style) and a role (What the cell represent)
USTRUCT(BlueprintType)
struct FMapSegmentSection
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Length = 0;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapRole Role = EMapRole::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme = "None";
};

// It represents a straight line of cells in the map graph. It has a strat coordinate, a direction and length. It is composed of sections that can
// define different themes in role in the same segment.
USTRUCT(BlueprintType)
struct FMapSegment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapGraphCoord Start = FMapGraphCoord::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection Direction = EMapDirection::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegmentSection> Sections;
	
	int32 GetLength() const;

	// Get all the cell of the segment
	TArray<FMapGraphCoord> GetCells() const;

	// Get the current section at the graph coordinate
	FMapSegmentSection GetSection(const FMapGraphCoord Coord) const;
	
	// Get the coordinate of the call at the segment index. Index represent the cell of the segment
	FORCEINLINE FMapGraphCoord GetCoordAt(const int32 Index) const
	{
		check(Index < GetLength());
		return Start.Stepped(Direction, Index);
	}
	
	bool IsValid() const { return GetLength() > 0 && Direction != EMapDirection::None; };

	void Print() const { UE_LOG(LogTemp, Warning, TEXT("Segment %d-%d | Length %d | Direction %s"),
		Start.Row, Start.Column, GetLength(), *MapUtils::GetDirectionText(Direction)); };
};

// Define branch generation behavior, with their direction, interval, number.
USTRUCT(BlueprintType)
struct FBranchRule
{
	GENERATED_BODY()

	// Direction relative to the main path 
	UPROPERTY(EditAnywhere)
	EMapTurn Turn = EMapTurn::None;

	UPROPERTY(EditAnywhere)
	int32 StepInterval = 0;

	// Percent probability of spawn from 0 to 1
	UPROPERTY(EditAnywhere)
	float SpawnProbability = 0.f;

	UPROPERTY(EditAnywhere)
	TArray<FMapSegmentSection> Sections;
};

// Main path of the map graph composed of segments.
USTRUCT(BlueprintType)
struct FMapMainPath
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegment> Segments;

	UPROPERTY(EditDefaultsOnly)
	FMapGraphCoord Start = FMapGraphCoord::None;

	// Get all cells of the path
	TArray<FMapGraphCoord> GetCells() const;

	// Get segment for an index of path.
	FMapSegment GetSegmentAt(int32 PathIndex) const;
	// Get segment for a coordinate of path.
	FMapSegment GetSegmentAt(FMapGraphCoord Cell) const;

	FORCEINLINE void Reset()
	{
		Segments.Reset();
		Start = FMapGraphCoord::None;
	};

	FORCEINLINE bool IsValid()
	{
		return Start != FMapGraphCoord::None && GetCells().Num() >= 2;
	}
};