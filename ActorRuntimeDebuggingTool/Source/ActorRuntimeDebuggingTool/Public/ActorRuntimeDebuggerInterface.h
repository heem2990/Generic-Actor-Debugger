// Heem.Patel 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "ActorRuntimeDebuggerInterface.generated.h"

class UActorRuntimeDebuggingComponent;

/** 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UActorRuntimeDebuggerInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IActorRuntimeDebuggerInterface
{
	GENERATED_IINTERFACE_BODY()


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Debugger")
	UActorRuntimeDebuggingComponent* GetRuntimeDebuggingComponent() const;
	virtual UActorRuntimeDebuggingComponent* GetRuntimeDebuggingComponent_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Debugger")
	void CollectData(UPARAM(ref)TArray<FString>& OutData) const ;
	virtual void CollectData_Implementation(TArray<FString>& OutData) const {}
};


UCLASS(meta = (ScriptName = "ActorRuntimeDebuggerLibrary"))
class UActorRuntimeDebuggerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Debugger")
	static void ToggleIfDebuggerPresent(AActor* InActor, bool bInEnable);
};
