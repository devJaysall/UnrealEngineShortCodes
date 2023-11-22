#pragma once

#include "LayerImageAsset.generated.h"


UCLASS(Blueprintable)
class ULayerImageAsset : public UObject
{  
   
    GENERATED_BODY()


public:

    UPROPERTY(EditDefaultsOnly)
    TArray<FSlateBrush> Layers; 

};