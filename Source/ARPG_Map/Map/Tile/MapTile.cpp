// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTile.h"

AMapTile::AMapTile()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GetComponents<UMapTileConnector>(Connectors);
}

