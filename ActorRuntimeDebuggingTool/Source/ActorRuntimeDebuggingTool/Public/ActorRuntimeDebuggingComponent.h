// Heem.Patel 2026
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"

#include "ActorRuntimeDebuggingComponent.generated.h"

class FGameplayDebuggerCanvasContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollectData, UActorRuntimeDebuggingComponent*, InDebugComponent);

UENUM(BlueprintType)
enum class ERuntimeDebuggerLineWarpHandling : uint8
{
	NoWarping, 
	WarpWithLineBreaksOnly,
	WarpWithLineBreaksAndIndent
};

UENUM(BlueprintType)
enum class ERuntimeDebuggerCollectDataType : uint8
{
	OnTick,
	OnTimer,
};

UCLASS(ClassGroup = (Debugger), meta = (BlueprintSpawnableComponent))
class ACTORRUNTIMEDEBUGGINGTOOL_API UActorRuntimeDebuggingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActorRuntimeDebuggingComponent();

public:
	virtual void DrawDebugger(FGameplayDebuggerCanvasContext& CanvasContext);
	virtual void CollectData();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	
	void DrawCallback(UCanvas* InCanvas, APlayerController* InPC);

	FOnCollectData& GetOnCollectDataDelegate() { return OnCollectDataDelegate; }

	UFUNCTION(BlueprintCallable)
	void AddToCollectedStrings(UPARAM(ref)TArray<FString>& StringsToAdd);

	UFUNCTION(BlueprintCallable)
	void ToggleDebuggerVisibility(const bool bInEnable);

private:

	void OnTimerElapsed();

	void WrapTextByWords(const FString& Text, int32 MaxCharsPerLine, FString& OutLineWithLineBreaksAdded);

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	ERuntimeDebuggerCollectDataType CollectDataFrequencyType{ ERuntimeDebuggerCollectDataType::OnTick };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true", EditCondition = "CollectDataFrequencyType==ERuntimeDebuggerCollectDataType::OnTimer", EditConditionHides))
	float CollectDataFrequencySeconds{ 0.2f };

	UPROPERTY(EditDefaultsOnly, Category="Debugger Data Options", meta=(AllowPrivateAccess="true"))
	bool bShowActorName { true };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Data Options", meta = (AllowPrivateAccess = "true"))
	bool bShowLocation { true };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	float HalfVerticalPadding{ 10.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	float HalfHorizontalPadding{ 10.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	float BorderThickness{ 1.5f };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	ERuntimeDebuggerLineWarpHandling LineWarpHandling{ ERuntimeDebuggerLineWarpHandling::WarpWithLineBreaksAndIndent };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true", EditCondition="LineWarpHandling!=ERuntimeDebuggerLineWarpHandling::NoWarping", EditConditionHides))
	float MaxWidth{ 300.0f };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true", EditCondition = "LineWarpHandling!=ERuntimeDebuggerLineWarpHandling::NoWarping", EditConditionHides))
	int MaxCharactersAllowedPerLine{ 50 };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	FLinearColor BackplateColor{ FLinearColor::White };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	FLinearColor BorderColor{ FLinearColor::Black};

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	FVector DebuggerOffset{ FVector::ZeroVector };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	bool bAddToVisualLogger{ true };

	UPROPERTY(EditDefaultsOnly, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	bool bBeginEnabled{ false };

	UPROPERTY(BlueprintAssignable, Category = "Debugger Display Settings", meta = (AllowPrivateAccess = "true"))
	FOnCollectData OnCollectDataDelegate;

	TArray<FString> CollectedData;
	bool bEnabled{ false };

	FDelegateHandle DebugDrawHandle;
	FTimerHandle TimerHandle;
};
