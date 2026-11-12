// Heem.Patel 2026

#include "ActorRuntimeDebuggerInterface.h"
#include "ActorRuntimeDebuggingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorRuntimeDebuggerInterface)

UActorRuntimeDebuggerInterface::UActorRuntimeDebuggerInterface(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UActorRuntimeDebuggerFunctionLibrary::ToggleIfDebuggerPresent(AActor* InActor, bool bInEnable)
{
	if (IsValid(InActor) && InActor->GetClass()->ImplementsInterface(UActorRuntimeDebuggerInterface::StaticClass()))
	{
		if (UActorRuntimeDebuggingComponent* FoundComponent = IActorRuntimeDebuggerInterface::Execute_GetRuntimeDebuggingComponent(InActor))
		{
			FoundComponent->ToggleDebuggerVisibility(bInEnable);
		}
	}

}
