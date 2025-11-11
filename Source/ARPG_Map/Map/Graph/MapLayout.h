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
	TArray<FMapSegment> PathSegments;
	
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

USTRUCT(BlueprintType)
struct FMapLayoutConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLayoutDefinition> AvailableLayouts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinSegmentLength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSegmentLength = 0;

	UPROPERTY(EditDefaultsOnly)
	FMapGraphCoord MainPathStart { 0, 0 };
	UPROPERTY(EditAnywhere)
	int32 MainPathLength = 0;
};