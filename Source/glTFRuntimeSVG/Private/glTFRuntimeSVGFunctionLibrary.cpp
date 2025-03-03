// Copyright 2025, Roberto De Ioris.


#include "glTFRuntimeSVGFunctionLibrary.h"

UTexture2D* UglTFRuntimeSVGFunctionLibrary::RenderSVGToTexture2D(UglTFRuntimeAsset* Asset, const int32 Width, const int32 Height, const EglTFRuntimeSVGsRGBMode SRGBMode, const FglTFRuntimeImagesConfig& ImagesConfig, const FglTFRuntimeTextureSampler& TextureSampler)
{
	const TArray64<uint8>& Blob = Asset->GetParser()->GetBlob();

	FglTFRuntimeMipMap MipMap(-1);
	if (!RenderSVGToMipMap(Blob, Width, Height, MipMap))
	{
		return nullptr;
	}

	TArray<FglTFRuntimeMipMap> Mips;
	Mips.Add(MoveTemp(MipMap));

	FglTFRuntimeImagesConfig TrueImagesConfig = ImagesConfig;

	if (SRGBMode != EglTFRuntimeSVGsRGBMode::FromImagesConfig)
	{
		TrueImagesConfig.bSRGB = SRGBMode == EglTFRuntimeSVGsRGBMode::Yes;
	}

	return Asset->GetParser()->BuildTexture(GetTransientPackage(), Mips, TrueImagesConfig, TextureSampler);
}

void UglTFRuntimeSVGFunctionLibrary::RenderSVGToTexture2DAsync(UglTFRuntimeAsset* Asset, const int32 Width, const int32 Height, const EglTFRuntimeSVGsRGBMode SRGBMode, const FglTFRuntimeImagesConfig& ImagesConfig, const FglTFRuntimeTextureSampler& TextureSampler, const FglTFRuntimeTexture2DAsync& OnTexture2D)
{
	Async(EAsyncExecution::Thread, [Asset, Width, Height, SRGBMode, ImagesConfig, TextureSampler, OnTexture2D]()
		{
			const TArray64<uint8> Blob = Asset->GetParser()->GetBlob();

			FglTFRuntimeMipMap MipMap(-1);
			if (!RenderSVGToMipMap(Blob, Width, Height, MipMap))
			{
				FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([OnTexture2D]()
					{
						OnTexture2D.ExecuteIfBound(nullptr);
					}, TStatId(), nullptr, ENamedThreads::GameThread);
				FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
				return;
			}

			TArray<FglTFRuntimeMipMap> Mips;
			Mips.Add(MoveTemp(MipMap));

			FglTFRuntimeImagesConfig TrueImagesConfig = ImagesConfig;

			if (SRGBMode != EglTFRuntimeSVGsRGBMode::FromImagesConfig)
			{
				TrueImagesConfig.bSRGB = SRGBMode == EglTFRuntimeSVGsRGBMode::Yes;
			}

			FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([Asset, &Mips, TrueImagesConfig, TextureSampler, OnTexture2D]()
				{
					OnTexture2D.ExecuteIfBound(Asset->GetParser()->BuildTexture(GetTransientPackage(), Mips, TrueImagesConfig, TextureSampler));
				}, TStatId(), nullptr, ENamedThreads::GameThread);
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
		});
}

bool UglTFRuntimeSVGFunctionLibrary::RenderSVGToMipMap(const TArray64<uint8>& Blob, const int32 Width, const int32 Height, FglTFRuntimeMipMap& MipMap)
{
	resvg_render_tree* SVGRenderTree = nullptr;

	if (resvg_parse_tree_from_data(reinterpret_cast<const char*>(Blob.GetData()), Blob.Num(), FglTFRuntimeSVGModule::SVGOptions, &SVGRenderTree) != RESVG_OK)
	{
		UE_LOG(LogGLTFRuntime, Error, TEXT("Unable to load the SVG blob"));
		return false;
	}

	resvg_size SVGSize = resvg_get_image_size(SVGRenderTree);

	int32 TrueWidth = Width;
	int32 TrueHeight = Height;

	if (TrueWidth <= 0)
	{
		TrueWidth = SVGSize.width;
	}

	if (TrueHeight <= 0)
	{
		TrueHeight = SVGSize.height;
	}

	float ScaleX = TrueWidth / SVGSize.width;
	float ScaleY = TrueHeight / SVGSize.height;

	resvg_transform SVGTransform = resvg_transform_identity();

	/*
		a b
		c d
		e f
	*/
	SVGTransform.a = ScaleX;
	SVGTransform.d = ScaleY;

	TArray64<uint8> Pixels;
	Pixels.AddZeroed(TrueWidth * TrueHeight * 4);

	resvg_render(SVGRenderTree, SVGTransform, TrueWidth, TrueHeight, reinterpret_cast<char*>(Pixels.GetData()));

	resvg_tree_destroy(SVGRenderTree);

	MipMap.PixelFormat = EPixelFormat::PF_R8G8B8A8;
	MipMap.Width = TrueWidth;
	MipMap.Height = TrueHeight;
	MipMap.Pixels = Pixels;

	return true;
}