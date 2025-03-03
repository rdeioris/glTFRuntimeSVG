// Copyright 2025, Roberto De Ioris.

#pragma once

#include "Modules/ModuleManager.h"
#include "resvg.h"

class FglTFRuntimeSVGModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static resvg_options* SVGOptions;
};
