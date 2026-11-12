// Heem.Patel 2026

#pragma once

#include "Modules/ModuleManager.h"

class FActorRuntimeDebuggingToolModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
