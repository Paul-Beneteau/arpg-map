# pragma once

#include "CoreMinimal.h"

enum class EMapBranchDirection : uint8;
enum class EMapDirection : uint8;

EMapDirection RotateDirectionLeft(EMapDirection Direction);

EMapDirection RotateDirectionRight(EMapDirection Direction);

EMapDirection GetBranchAbsoluteDirection(EMapDirection Direction, EMapBranchDirection BranchDirection);
