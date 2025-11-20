# pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/Types/MapTypes.h"

struct FMapSegment;
struct FMapGraph;

namespace MapUtils
{
	int32 RotationToYaw(EMapRotation Rotation);
	
	// Rotate a direction clockwise by the specified rotation. RotateClockwise(North, Degree90) -> East
	EMapDirection RotateClockwise(EMapDirection Direction, EMapRotation Rotation);

	// Get the rotation between two direction
	EMapRotation GetRotationBetween(EMapDirection FromDirection, EMapDirection TowardDirection);
	
	// Get opposite direction
	FORCEINLINE EMapDirection Opposite(EMapDirection Direction)
	{
		return RotateClockwise(Direction, EMapRotation::Degree180);
	}
	
	// Get both perpendicular direction
	FORCEINLINE TArray<EMapDirection> Perpendicular(EMapDirection Direction)
	{
		return { RotateClockwise(Direction, EMapRotation::Degree270), RotateClockwise(Direction, EMapRotation::Degree90) };
	}
	
	// Get the direction of a cell toward another cell
	EMapDirection GetDirectionToward(const FMapGraphCoord& FromCell, const FMapGraphCoord& TowardCell);
	// Get the direction toward the center of the graph
	EMapDirection GetInwardDirection(const FMapGraphCoord& Coord, const int32 Rows, const int32 Columns);

	FORCEINLINE FString GetDirectionText(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return "North";
		
		case EMapDirection::West:
			return "West";
		
		case EMapDirection::South:
			return "South";

		case EMapDirection::East:
			return "East";
		
		default:
			return "None";
		}
	}

	FORCEINLINE bool IsInsideBounds(const int32 Rows, const int32 Columns, const FMapGraphCoord& Coord)
	{
		return Coord.Row >= 0 && Coord.Row < Rows	&& Coord.Column >= 0 && Coord.Column < Columns;
	}

	int32 GetPathLength(const TArray<FMapSegment>& Path);
	
	void PrintGraph(FMapGraph& MapGraph, const UWorld* InWorld, const int32 CellSize);

	template<typename T>
	T* PickWeightedRandom(const TArray<T*>& Items, TFunction<float(const T*)> GetWeight)
	{
		float TotalWeight = 0.f;		
		for (const T* Item : Items)
		{
			if (Item)
				TotalWeight += GetWeight(Item);
		}

		if (TotalWeight <= 0.0f)
		{
			UE_LOG(LogTemp, Error, TEXT("PickWeightedRandom: Total weight is 0"));
			return nullptr;
		}
		
		float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
		float CurrentWeight = 0.0f;
    
		for (T* Item : Items)
		{
			CurrentWeight += GetWeight(Item);
			if (RandomWeight <= CurrentWeight)
				return Item;
		}
    
		return nullptr;
	}

	template<typename T>
	T PickWeightedRandom(const TArray<T>& Items, TFunction<float(const T&)> GetWeight)
	{
		float TotalWeight = 0.f;		
		for (const T& Item : Items)
			TotalWeight += GetWeight(Item);

		if (TotalWeight <= 0.0f)
		{
			UE_LOG(LogTemp, Error, TEXT("PickWeightedRandom: Total weight is 0"));
			return T();
		}
		
		float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
		float CurrentWeight = 0.0f;
    
		for (const T& Item : Items)
		{
			CurrentWeight += GetWeight(Item);
			if (RandomWeight <= CurrentWeight)
				return Item;
		}
    
		return T();
	}
}

