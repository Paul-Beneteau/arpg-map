#pragma once

#include "CoreMinimal.h"
#include "MapGraphUtils.h"
#include "MapTypes.h"
#include "MapLayout.generated.h"

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

USTRUCT(BlueprintType)
struct FMapSegmentSection
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 Length = 0;
		
	UPROPERTY(EditAnywhere)
	EMapRole Role = EMapRole::None;
	
	UPROPERTY(EditAnywhere)
	FName Theme = "None";
};

USTRUCT(BlueprintType)
struct FMapSegment
{
	GENERATED_BODY()

	FMapGraphCoord Start { FMapGraphCoord::None };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection Direction { EMapDirection::None };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegmentSection> Sections;
	
	int32 GetLength() const;

	FMapGraphCoord GetCoordAt(int32 SegmentIndex) const
	{
		check(SegmentIndex < GetLength());
		return Start.Stepped(Direction, SegmentIndex);
	}

	TArray<FMapGraphCoord> GetCells() const;
	
	bool IsValid() const { return GetLength() > 0 && Direction != EMapDirection::None; };

	void Print() const { UE_LOG(LogTemp, Warning, TEXT("Segment %d-%d | Length %d | Direction %s"),
		Start.Row, Start.Column, GetLength(), *MapUtils::GetDirectionText(Direction)); };
};

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

USTRUCT(BlueprintType)
struct FMapMainPath
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegment> Segments;

	UPROPERTY(EditDefaultsOnly)
	FMapGraphCoord Start { FMapGraphCoord::None };

	TArray<FMapGraphCoord> GetCells() const;

	FMapSegment GetSegmentAt(int32 PathIndex) const;
	
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