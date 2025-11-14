#pragma once

#include "ARPG_Map/Map/Graph/MapLayout.h"
#include "MapLayoutGenerator.generated.h"

// Represent the map layout. It is composed of a main path which has the shape of EMapLayout, and branch rules that
// determine how branches are generated from the main path.
USTRUCT(BlueprintType)
struct FMapLayout
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapMainPath MainPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBranchRule> BranchRules;
	
	FORCEINLINE void Reset()
	{
		MainPath.Reset();
		BranchRules.Reset();
	};

	FORCEINLINE bool IsValid()
	{
		return MainPath.IsValid();
	}
};

// Configurations to generate a layout.
USTRUCT(BlueprintType)
struct FMapLayoutConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapLayout Layout { EMapLayout::None };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SegmentMinLength { 0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SegmentMaxLength { 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Themes;
};

// Generate a map layout composed of the main path and his branches based on FMapLayoutConfig.
USTRUCT(Blueprintable)
struct FMapLayoutGenerator
{
	GENERATED_BODY()

public:
	// Default constructor needed by Unreal
	FMapLayoutGenerator() = default;
	
	static FMapLayout GenerateLayout(const FMapLayoutConfig& InConfig, const int32 InRows, const int32 InColumns);

protected:
	FMapLayoutConfig Config;
	
	int32 Rows { 0 };
	int32 Columns { 0 };

	FMapLayoutGenerator(const FMapLayoutConfig& InConfig, int32 InRows, int32 InColumns);

	// Layout generators for each shape
	FMapLayout GenerateLayoutL();	
	FMapLayout GenerateLayoutU();
	
	void GenerateStartCell(FMapLayout& Layout);

	// Generate a segment with one of the direction of PossibleDirections. The segment starting coordinate are the
	// anchor coordinates stepped by the AnchorOffset in the segment direction. If the Length parameter is not
	// specified, a random length is chosen.
	FMapSegment GenerateSegment(
		const TArray<EMapDirection>& PossibleDirections,
		const FMapGraphCoord Anchor,
		const int32 AnchorOffset = 0,
		const int32 Length = -1);

	// Filter segment valid direction from given directions
	TArray<EMapDirection> FilterValidDirections(
		const FMapGraphCoord SegmentStart,
		const int32 SegmentLength,
		const TArray<EMapDirection>& Directions) const;
};

