#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapTileTemplate.generated.h"

enum class EMapDirection : uint8;
class AMapTile;

UCLASS()
class ARPG_MAP_API UMapTileTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMapTile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 0.f;

	bool HasConnectors(const TArray<EMapDirection>& Connectors);
};
