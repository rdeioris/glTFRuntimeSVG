// Copyright 2025, Roberto De Ioris.

#include "glTFRuntimeSVG.h"

#define LOCTEXT_NAMESPACE "FglTFRuntimeSVGModule"

resvg_options* FglTFRuntimeSVGModule::SVGOptions = nullptr;

void FglTFRuntimeSVGModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	SVGOptions = resvg_options_create();

	if (SVGOptions)
	{
		resvg_options_load_system_fonts(SVGOptions);
	}
}

void FglTFRuntimeSVGModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (SVGOptions)
	{
		resvg_options_destroy(SVGOptions);
		SVGOptions = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FglTFRuntimeSVGModule, glTFRuntimeSVG)