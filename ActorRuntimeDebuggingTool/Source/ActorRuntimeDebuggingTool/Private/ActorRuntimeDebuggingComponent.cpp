// Heem.Patel 2026

#include "ActorRuntimeDebuggingComponent.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "GameplayDebuggerTypes.h"
#include "Debug/DebugDrawService.h"
#include "Logging/LogMacros.h"
#include "ActorRuntimeDebuggerInterface.h"

TAutoConsoleVariable<bool> CVarEnableActorDebugger (
	TEXT("ActorDebugger.Enable"),
	true,
	TEXT("enables the actor debugger")
);

DEFINE_LOG_CATEGORY_STATIC(LogRuntimeDebugComponent, Log, All);

UActorRuntimeDebuggingComponent::UActorRuntimeDebuggingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled= false;

	bAutoRegister = true;
	bEnabled = false;
}

void UActorRuntimeDebuggingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bBeginEnabled)
	{
		//Enable it.
		constexpr bool bEnableDebugger = true;
		ToggleDebuggerVisibility(bEnableDebugger);
	}
}

void UActorRuntimeDebuggingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TimerHandle.IsValid())
	{
		if (UWorld* CurrentWorld = GetWorld())
		{
			CurrentWorld->GetTimerManager().ClearTimer(TimerHandle);
		}
	}

	if (DebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UActorRuntimeDebuggingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CVarEnableActorDebugger.GetValueOnGameThread() && bEnabled)
	{
		CollectData();
	}
}

void UActorRuntimeDebuggingComponent::AddToCollectedStrings(TArray<FString>& InStringsToAdd)
{
	CollectedData.Append(InStringsToAdd);
}

void UActorRuntimeDebuggingComponent::ToggleDebuggerVisibility(const bool bInEnable)
{
	if (bEnabled != bInEnable)
	{
		bEnabled = bInEnable;
		
		if (CollectDataFrequencyType == ERuntimeDebuggerCollectDataType::OnTick)
		{
			SetComponentTickEnabled(bInEnable);
		}
		else
		{
			if (bInEnable && !TimerHandle.IsValid())
			{
				if (UWorld* CurrentWorld = GetWorld())
				{
					FTimerManagerTimerParameters TimerParams;
					TimerParams.bLoop = true;
					TimerParams.bMaxOncePerFrame = true;
					TimerParams.FirstDelay = 0.0f;

					CurrentWorld->GetTimerManager().SetTimer(TimerHandle, this, &UActorRuntimeDebuggingComponent::OnTimerElapsed, CollectDataFrequencySeconds, TimerParams);
				}
			}
			else if (!bInEnable && TimerHandle.IsValid())
			{
				if (UWorld* CurrentWorld = GetWorld())
				{
					CurrentWorld->GetTimerManager().ClearTimer(TimerHandle);
				}
			}
		}

		if (bEnabled && !DebugDrawHandle.IsValid())
		{
			DebugDrawHandle = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UActorRuntimeDebuggingComponent::DrawCallback));
		}
		else if (!bEnabled && DebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(DebugDrawHandle);
		}

	}
}

void UActorRuntimeDebuggingComponent::OnTimerElapsed()
{
	if (bEnabled)
	{
		CollectData();
	}
}

void UActorRuntimeDebuggingComponent::DrawCallback(UCanvas* InCanvas, APlayerController* InPC)
{
	if (!CVarEnableActorDebugger.GetValueOnGameThread())
	{
		return;
	}

	if (!IsValid(InCanvas))
	{
		return;
	}

	if (!IsValid(InPC))
	{
		return;
	}

	FGameplayDebuggerCanvasContext CanvasContext(InCanvas, GEngine->GetMediumFont());
	CanvasContext.World = InPC->GetWorld();

	DrawDebugger(CanvasContext);

}

void UActorRuntimeDebuggingComponent::DrawDebugger(FGameplayDebuggerCanvasContext& InDebugCanvasContext)
{
	if (!bEnabled)
	{
		return;
	}

	const AActor* MyOwner = GetOwner();
	if (!IsValid(MyOwner))
	{
		return;
	}

	if (CollectedData.IsEmpty())
	{
		return;
	}
		
	const FVector WorldSpaceDebuggerLocation = MyOwner->GetActorLocation() + DebuggerOffset;
	if (!InDebugCanvasContext.IsLocationVisible(WorldSpaceDebuggerLocation))
	{
		return;
	}

	//Get World location in screen space. 
	const FVector2D ScreenPosition = InDebugCanvasContext.ProjectLocation(WorldSpaceDebuggerLocation);

	float WidestStringMeasure = 0.0f;
	float RequiredHeight = 0.0f;

	for (const FString& CurrEntry : CollectedData)
	{
		float StrPixelHeight{ 0.0f };
		float StrPixelWidth{ 0.0f };

		//Handle Warping.
		FString WrappedLines;
		WrapTextByWords(CurrEntry, MaxCharactersAllowedPerLine, WrappedLines);

		InDebugCanvasContext.MeasureString(WrappedLines, StrPixelWidth, StrPixelHeight);

		WidestStringMeasure = FMath::Max(WidestStringMeasure, StrPixelWidth);
		RequiredHeight = RequiredHeight + StrPixelHeight;
	}

	//If no line warping. Adjust to longest string.
	float RequiredWidth = WidestStringMeasure;
	if (LineWarpHandling == ERuntimeDebuggerLineWarpHandling::WarpWithLineBreaksOnly)
	{
		RequiredWidth = MaxWidth;
	}
	else if (LineWarpHandling == ERuntimeDebuggerLineWarpHandling::WarpWithLineBreaksAndIndent)
	{
		//Add a bit of offset to account for space needed for indentation
		constexpr float PaddingToOffsetIndent{ 20.0f };
		RequiredWidth = MaxWidth + PaddingToOffsetIndent;
	}
		
	const float DebuggerWidth = RequiredWidth + (2.0f * HalfHorizontalPadding);
	const float DebuggerHeight = RequiredHeight + (2.0f * HalfVerticalPadding);

	const FVector2D DiagonalHalfOffset(DebuggerWidth * 0.5f, DebuggerHeight * 0.5f);

	const FVector2D BoxExtentMin = ScreenPosition - DiagonalHalfOffset;
	const FVector2D BoxExtentMax = ScreenPosition + DiagonalHalfOffset;

	const FBox2D DebugBoxExtents(BoxExtentMin, BoxExtentMax);

	FCanvasTileItem BackgroundBox = FCanvasTileItem(DebugBoxExtents.GetCenter(), DebugBoxExtents.GetSize(), BackplateColor);
	BackgroundBox.BlendMode = SE_BLEND_Translucent;

	InDebugCanvasContext.DrawItem(BackgroundBox, BoxExtentMin.X, BoxExtentMin.Y);

	// Border
	if (BorderThickness > 0.0f)
	{
		FCanvasBoxItem BoxItem(DebugBoxExtents.GetCenter(), DebugBoxExtents.GetSize());
		BoxItem.SetColor(BorderColor);
		BoxItem.LineThickness = BorderThickness;
		InDebugCanvasContext.DrawItem(BoxItem, BoxExtentMin.X, BoxExtentMin.Y);
	}

	// Draw all the text
	const FVector2D DebugBoxCenter = DebugBoxExtents.GetCenter();
	float RunningYOffset = BoxExtentMin.Y + HalfVerticalPadding;
	const float EntryXLoc = BoxExtentMin.X + HalfHorizontalPadding;

	for (int32 i = 0; i < CollectedData.Num(); ++i)
	{
		const FString& CurrEntry = CollectedData[i];

		//Handle Warping.
		FString WrappedLines;
		WrapTextByWords(CurrEntry, MaxCharactersAllowedPerLine, WrappedLines);

		float CurrEntryWidth{ 0.0f }, CurrEntryHeight{ 0.0f };
		InDebugCanvasContext.MeasureString(WrappedLines, CurrEntryWidth, CurrEntryHeight);

		const float EntryYLoc = RunningYOffset;
		InDebugCanvasContext.PrintAt(EntryXLoc, EntryYLoc, WrappedLines);

		RunningYOffset = RunningYOffset + CurrEntryHeight;

		if (bAddToVisualLogger)
		{
			UE_VLOG(GetOwner(), LogRuntimeDebugComponent, Log, TEXT("%s"), *CurrEntry);
		}
	}
}

void UActorRuntimeDebuggingComponent::WrapTextByWords(const FString& Text, int32 MaxCharsPerLine, FString& OutLineWithLineBreaksAdded)
{
	if (Text.Len() <= MaxCharsPerLine || LineWarpHandling == ERuntimeDebuggerLineWarpHandling::NoWarping)
	{
		OutLineWithLineBreaksAdded = Text;
		return;
	}

	// Split the text by spaces
	TArray<FString> Words;
	Text.ParseIntoArray(Words, TEXT(" "), true);
	if (Words.Num() == 0)
	{
		OutLineWithLineBreaksAdded = Text;
		return;
	}

	FString CurrentLine = Words[0];
	TArray<FString> WrappedLines;

	for (int32 i = 1; i < Words.Num(); ++i)
	{
		// Check if adding the next word exceeds the limit
		if ((CurrentLine.Len() + 1 + Words[i].Len()) <= MaxCharsPerLine)
		{
			CurrentLine += TEXT(" ") + Words[i];
		}
		else
		{
			// Current line is full, push it and start a new one
			WrappedLines.Add(CurrentLine);
			CurrentLine = Words[i];
		}
	}

	// Add the final remaining line
	if (!CurrentLine.IsEmpty())
	{
		WrappedLines.Add(CurrentLine);
	}

	if (LineWarpHandling == ERuntimeDebuggerLineWarpHandling::WarpWithLineBreaksOnly)
	{
		OutLineWithLineBreaksAdded = FString::Join(WrappedLines, TEXT("\n"));
	}
	else if(LineWarpHandling == ERuntimeDebuggerLineWarpHandling::WarpWithLineBreaksAndIndent)
	{
		OutLineWithLineBreaksAdded = FString::Join(WrappedLines, TEXT("\n\t"));
	}
}

void UActorRuntimeDebuggingComponent::CollectData()
{
	CollectedData.Reset();
	const AActor* MyOwner = GetOwner();
	if (!IsValid(MyOwner))
	{
		return;
	}

	if (bShowActorName)
	{
		const FString NameString = FString::Printf(TEXT("{blue}Name: {black}%s"), *GetNameSafe(MyOwner));
		CollectedData.Add(NameString);
	}

	if (bShowLocation)
	{
		const FVector OwnerLoc = MyOwner->GetActorLocation();
		const FString NameString = FString::Printf(TEXT("{blue}Loc: {black}%s"), *(OwnerLoc.ToCompactString()));
		CollectedData.Add(NameString);
	}

	if (MyOwner->GetClass()->ImplementsInterface(UActorRuntimeDebuggerInterface::StaticClass()))
	{
		// Call the interface function using the generated Execute_ macro
		TArray<FString> DataFromInterface;
		IActorRuntimeDebuggerInterface::Execute_CollectData(MyOwner, DataFromInterface);

		CollectedData.Append(DataFromInterface);
	}

	OnCollectDataDelegate.Broadcast(this);
}