#pragma once

#include "CoreMinimal.h"
#include "MapTypes.h"
#include "MapLayout.generated.h"

UENUM(BlueprintType)
enum class EMapLayout : uint8
{
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection Direction = EMapDirection::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegmentSection> Sections;

	int32 GetLength() const;
};

USTRUCT(BlueprintType)
struct FLayoutDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapLayout Layout = EMapLayout::Straight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegment> MainPathSegments;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowMirrorHorizontal = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowMirrorVertical = true;
};

USTRUCT(BlueprintType)
struct FLayoutInstance
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapLayout Layout = EMapLayout::Straight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapSegment> Segments;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlippedH = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlippedV = false;
};

// Branch direction relative to the main path 
UENUM(BlueprintType)
enum class EMapBranchDirection : uint8
{
	None,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FBranchRule
{
	GENERATED_BODY()

	// Direction relative to the main path 
	UPROPERTY(EditAnywhere)
	EMapBranchDirection RelativeDirection = EMapBranchDirection::None;

	UPROPERTY(EditAnywhere)
	int32 StepInterval = 0;

	// Percent probability of spawn from 0 to 1
	UPROPERTY(EditAnywhere)
	float SpawnProbability = 0.f;

	UPROPERTY(EditAnywhere)
	TArray<FMapSegmentSection> Sections;
};

USTRUCT(BlueprintType)
struct FMapLayoutConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLayoutDefinition> AvailableLayouts;

	UPROPERTY(EditDefaultsOnly)
	FMapGraphCoord MainPathStart { 0, 0 };

	UPROPERTY(EditAnywhere)
	TArray<FBranchRule> BranchRules;
};