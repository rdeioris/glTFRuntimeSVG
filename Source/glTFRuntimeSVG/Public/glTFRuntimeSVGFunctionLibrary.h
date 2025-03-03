// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeAsset.h"
#include "glTFRuntimeSVG.h"
#include "glTFRuntimeSVGFunctionLibrary.generated.h"

UENUM()
enum class EglTFRuntimeSVGsRGBMode : uint8
{
	Yes,
	No,
	FromImagesConfig
};

/**
 *
 */
UCLASS()
class GLTFRUNTIMESVG_API UglTFRuntimeSVGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "ImagesConfig,TextureSampler"), Category = "glTFRuntime|SVG")
	static UTexture2D* RenderSVGToTexture2D(UglTFRuntimeAsset* Asset, const int32 Width, const int32 Height, const EglTFRuntimeSVGsRGBMode SRGBMode, const FglTFRuntimeImagesConfig& ImagesConfig, const FglTFRuntimeTextureSampler& TextureSampler);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "ImagesConfig,TextureSampler"), Category = "glTFRuntime|SVG")
	static void RenderSVGToTexture2DAsync(UglTFRuntimeAsset* Asset, const int32 Width, const int32 Height, const EglTFRuntimeSVGsRGBMode SRGBMode, const FglTFRuntimeImagesConfig& ImagesConfig, const FglTFRuntimeTextureSampler& TextureSampler, const FglTFRuntimeTexture2DAsync& OnTexture2D);

	static bool RenderSVGToMipMap(const TArray64<uint8>& Blob, const int32 Width, const int32 Height, FglTFRuntimeMipMap& MipMap);
};
