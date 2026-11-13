# Actor Runtime Debugging Tool

A runtime debugger tool for UE5 that draws debug information strings for any actor, at an offset from it. It is highly customizable for any actor type, you can add the strings that are relevant for debugging, and can see them on the debugger over that actor. It also provides an option to forward the strings to visual logger, which provides a scrubbable history of any important debug state you might want to track on different actors/characters. 

<img width="1081" height="581" alt="Actor Runtime Debugger overview" src="https://github.com/user-attachments/assets/bd9f21bc-94b7-446f-8270-68f42a7c6f98" />

> *Example of two different actors using the runtime debugging tool*

## Table of Contents

- [Why?](#why)
- [Part 1: Using the Debugger](#part-1-using-the-debugger)
  - [Overview](#overview)
  - [Setup](#setup)
  - [Adding Dynamic Strings](#adding-dynamic-strings)
  - [Toggling Visibility of the debugger](#toggling-visibility-of-the-debugger)
  - [Options available for the debugger](#options-available-for-the-debugger)
- [Part 2: How to build a debugger like this](#part-2-how-to-build-a-debugger-like-this)
  - [Collecting Data](#collecting-data)
  - [Drawing Data](#drawing-data)
  - [Handling Line Wrapping](#handling-line-wrapping)
- [Contact](#contact)

## Why?

UE5 already ships with several excellent debuggers like the Gameplay Debugger, Visual Logger and Rewind Debugger. They are all incredibly helpful in appropriate situations, however they each have limitations:

- `Gameplay Debugger` can only be enabled on one actor at a time.
- Adding information to the `Gameplay Debugger` may require creating a new debugger, if it doesnt exist, which can be a lot of setup, when all you might want is to see some live data on the actor. And it is also not the most design friendly setup.
- For `Rewind Debugger, and Visual Logger`, you need to be recording, and you need to have the correct debug actor selected, although they are scrubbable, which is a huge plus. 
- `GEngine->AddOnScreenDebugMessage` works well, but draws in screen space so it can quickly become overwhelming if theres multiple pieces of data and multiple actors. Additionally, because its on screen space, its not easy to know what data might be associated with what actor.


> **Note:** This tutorial is divided into two parts.
> **Part 1** shows how to take this debugger and adapt it to your project, and what considerations to keep in mind when adapting to your project. 
> **Part 2** walks through the process of making a debugger like this.

---

# Part 1: Using the Debugger

## Overview

| Part | Role |
| --- | --- |
| **Debugger Component** (`ActorRuntimeDebuggingComponent`) | This is the component that should be added to the actor you want to draw this debugger on. It can be added natively, or in a Blueprint class. It is basically responsible for collecting and drawing all the the data, and the debugger. It also has settings that lets you manipulate the appearance of the debugger, and some of the behavior such as how often it collects data.|
| **Debugger Interface** (`ActorRuntimeDebuggerInterface`) | This is the interface that should be added, either natively on in blueprint class, to the actor that you want to draw the debugger on. It provides the function `CollectData` that can be overriden to populate the debug string. Additionally it also provides an overrideable getter so that external systems can interface with the component as required. |

## Setup
### 1. Setting up the Plugin

Add this plugin under your `...Projects/Plugin` folder. You'll want to reference this plugin in the build.cs file of your project, typically under `.../Source/Project/` and will be named as Project.build.cs. In that add `ActorRuntimeDebuggingTool` in the PublicDependencyModuleNames array. This will allow you to reference this plugin in your project.  

### 2. Add the component

Add `ActorRuntimeDebuggingComponent` to the actor you want to debug, this can be either in Blueprint or in a native C++ class.

You can quickly test at this point: enable **BeginEnabled** on the component, then start PIE. The actor should draw a debugger that shows the name of the actor, and its location.

> NOTE: If nothing appears, make sure the CVar `ActorDebugger.Enable` is enabled. It is the main gate that enables or disables this debugger.

<img width="975" height="279" alt="Component added to an actor in Blueprint" src="https://github.com/user-attachments/assets/1f247635-0c79-4a54-9d5c-fe41eaec07a2" />

> *Example of the component and interface added to an actor in Blueprint*

<img width="975" height="420" alt="Component added to an actor in C++" src="https://github.com/user-attachments/assets/21f7b548-48b0-4759-b1f8-eed9e037b5cd" />

> *Example of the component and interface added to an actor in C++*

### 3. Implement the interface

The actor needs to implement `ActorRuntimeDebuggerInterface`, either in Blueprint (via **Class Settings**) or natively. The interface exposes functions to get the debugger component and to implement `CollectData`, which lets you add whatever debug strings you want.

> **Tip:** You can colorize strings with `{color}` tags. Everything after a color tag will be drawn in that color. Available colors are listed in `ColorList.h` (around 100 of them).

### 4. Override `CollectData`

Once the interface is implemented, override `CollectData` (or `CollectData_Implementation` in C++) on the actor to add strings that you want to show on the debugger at runtime. This `CollectData` function will get called either every tick, or on timer depending on the settings in the component.

> **Note:** When implementing `CollectData` in Blueprint, make sure to call the **Parent** `CollectData`. That way, if the function is also implemented natively, both the native and the blueprint versions will get called.

<img width="975" height="506" alt="Adding a debug string in Blueprint" src="https://github.com/user-attachments/assets/b8a0afde-b464-4c08-bb06-c1d083682ad8" />

> *Adding a debug string in Blueprint (the output is visible on the Manny character)*

<img width="975" height="414" alt="Adding a debug string natively" src="https://github.com/user-attachments/assets/a124ce40-c5f9-4932-ade7-8674d2769701" />

> *Adding a debug string natively (the output is visible on the Manny character)*

## Adding Dynamic Strings

### From external systems

To add debug strings from an external trigger (E.g. an actor moving in or out of the range of an interactable), use the **`CollectData Delegate`** exposed by the component. It is called whenever the component is collecting data, it provides the component as a parameter, and you can add strings to the component directly via calling `AddToCollectedStrings` function on the component.

<img width="975" height="492" alt="Trigger volume adding a dynamic string" src="https://github.com/user-attachments/assets/892b7d77-38c3-410f-b82f-0251d9e3b8d3" />

> *A Trigger Volume adding a dynamic string whenever an actor starts overlapping it*

### For debug strings you might want on ALL actors

To print information for **all** actors that use this component, I recommend adding that directly in the `ActorRuntimeDebuggingComponent` component's own `CollectData` function. 

<img width="975" height="567" alt="Adding strings in the component's CollectData" src="https://github.com/user-attachments/assets/94823526-7ea6-45e8-94b4-045ae864f090" />

## Toggling Visibility of the debugger

You have a couple of options for toggling the debugger:

1. **`BeginEnabled`** on the component: when enabled, the debugger will be shown automatically whenever the actor owning that component begins play.
2. **`ToggleIfDebuggerPresent`** - This is a Blueprint-exposed function on the `ActorRuntimeDebuggingComponent` that can be called to enable or disable the visibility of the debugger dynamically. You can attach it to a key press, toggle it via controller input, or some console command depending on the needs of your project.

<img width="975" height="346" alt="Toggling debugging dynamically" src="https://github.com/user-attachments/assets/8a4d0984-d5a2-4270-98ef-1791438894da" />

> *Toggling debugging dynamically*

## Options available for the debugger

<img width="975" height="734" alt="Cosmetic options in the component details panel" src="https://github.com/user-attachments/assets/ee13534a-fb54-4f74-98fc-4361b006b807" />

You can control how and when the debugger is drawn. This is especially handy when enabling debuggers for multiple actor types, as simple color changes can help you quickly identify which actor a debugger belongs to.

| Option | Description |
| --- | --- |
| **CollectDataFrequencyType** | Whether the component should collect data on Timer or on Tick. |
| **CollectDataFrequencySeconds** | If collecting data on Timer, how frequently to collect data (in seconds). |
| **HalfVerticalPadding** | Padding around the text at the top/bottom of the debugger. |
| **HalfHorizontalPadding** | Padding aruond the text at the left/right of the debugger |
| **BorderThickness** | Thickness of the border (`0.0` is no border). |
| **LineWrapHandling** | How the debugger handles long strings (see below). `NoWrapping`, `WrapWithLineBreaksOnly`, `WrapWithLineBreaksAndIndent`|
| **MaxWidth** | Maximum width of the debugger (relevant when wrapping lines). |
| **MaxCharactersAllowedPerLine** | Characters allowed before wrapping (relevant when wrapping lines). |
| **BackplateColor** | Color of the debugger's backplate (supports translucency). |
| **BorderColor** | Color of the debugger's border. |
| **DebuggerOffset** | Offset from the actor's world location (projected onto the screen) at which the debugger is drawn. To place it above a character, add the appropriate value to Z. |
| **BeginEnabled** | If set, the debugger enables itself on Begin Play. |
| **AddToVisualLogger** | If enabled, also adds the text to the Visual Logger. |

**LineWrapHandlingModes:**

- `NoWrapping`: strings are printed as-is, and the debugger sizes itself to the widest string.
- `WrapWithLineBreaksOnly`: lines exceeding the max characters are wrapped with a line break.
- `WrapWithLineBreaksAndIndent`: lines exceeding the max characters are wrapped with a line break and an indent.


---

# Part 2: How to build a debugger like this

A debugger basically has two parts:

1. **How does it get the data it prints?**
   There are many approaches. The Gameplay Debugger requires every debugger type to implement a `CollectData` function. The Visual Logger lets you log from anywhere using `UE_VLOG` macros, and then it collates the information by actor and channel. The Runtime Actor Debugger is modelled after the Gameplay Debugger, but only takes strings as input, since strings are all that this tool prints on screen.

2. **How does it draw the data it has?**
   This also varies with the type of debugger. The Gameplay Debugger exposes a canvas to draw text and shapes in world or screen space. The Visual Logger needs a more bespoke editor. Since the Runtime Actor Debugger is modelled after the Gameplay Debugger, it also draws on a canvas, and makes use of the `GameplayDebuggerCanvasContext` because it provides a bunch of neat wrapper functions. 

## Collecting Data

Ideally, any type of actor should be able to quickly add this debugger and start adding debug information to it. Making it an `ActorComponent` provides that flexibility, but the actor itself still needs an easy way to add the strings it needs to.

**Why an interface?**

- It can be implemented in Blueprint, natively, or both. Tech designers can add the debugger to any new Blueprint actor without involving engineering, and engineers can add strings natively as they build new systems.
- The debugger component stays agnostic of the actor type implementing the interface. Any other system can interact with the debugger solely through the interface functions, without requiring any knowledge of the actor type that is making use of the component.

**Delegate:** In addition to the interface, the component has a delegate that gets broadcast at the end of `CollectData` function. Any system can subscribe to the delegate, and it'll get a callback when the data is being collected by the debugger. That callback is its opportunity to push its own strings. E.g. other components on the actor can get the debugger component from their owner, subscribe to the delegate, and push debug messages about their own state.

**Generic messages:** The component itself adds two generic messages: the actor name and its location. A project that needs to expose more generic information that is applicable to all actors using the debugger, should do so in the function `ActorRuntimeDebuggingComponent::CollectData`.

<img width="975" height="600" alt="The component's CollectData function" src="https://github.com/user-attachments/assets/a9542568-ffdb-46d5-91eb-b2a6ee0d8d54" />

> *Collect Data Function of the component*

**When does `CollectData` get called?** There are two options on the component: on tick, or on a timer. However, you can easily extend this, and all you need to do is to make sure to the call the function `ActorRuntimeDebuggingComponent::CollectData` from wherever is appropriate for your project.

## Drawing Data

### Getting a canvas and a draw callback

`UDebugDrawService` provides both a callback when the game is drawn and a canvas to draw to. It is also what the Gameplay Debugger uses to register its draw call, and is pretty simple to use.

`ActorRuntimeDebuggerComponent` registers with `UDebugDrawService` and receives the `ActorRuntimeDebuggerComponent::DrawCallback` function, which is called with a `Canvas` and `PlayerController` parameters can be used as necessary. The `ActorRuntimeDebuggerComponent` registers and unregisters form the debug service whenever its visibility is toggled. 

<img width="975" height="581" alt="Registering with UDebugDrawService" src="https://github.com/user-attachments/assets/22c4b93d-1042-4be4-9c7c-fa70ef9fab52" />

> *Toggle visibility function that also registers/unregisters the callbacks*

### Wrapping in Gameplay Debugges Canvas Context

The component wraps the `UCanvas` in an `FGameplayDebuggerCanvasContext`. It provides a really handy wrapper functions around the `UCanvas` draw functions that make drawing text easier. Text coloring is also handled by `FGameplayDebuggerCanvasContext` which is pretty handy..

<img width="975" height="115" alt="Wrapping UCanvas in FGameplayDebuggerCanvasContext" src="https://github.com/user-attachments/assets/2a39c069-7b77-4b61-a276-3865c821ca31" />

> *Wrapping the canvas in FGameplayDebuggerCanvasContext*

### Drawing the Debugger

The `FGameplayDebuggerCanvasContext` is then used to draw the debugger:

1. **Determine where to draw.** - Calculate the `WorldLocation`, the component does this by adding the `DebuggerOffset` specified in the component to the actor's world space location. Then it uses the canvas context to project it to screen-space location. If the world location is not visible on screen, it doesnt get is drawn. There is a handy function for this `FGameplayDebuggerCanvasContext::IsLocationVisible`.
2. **Measure the debugger size.** The component then measures how big it needs the debugger to be. We can make use of the handy wrapper function `FGameplayDebuggerCanvasContext::MeasureString` for doing this. It outputs how wide and high the string will be when rendered. We also need to determine if any of the entry needs to warp on line before measure the string.

<img width="975" height="522" alt="Measuring the debugger size" src="https://github.com/user-attachments/assets/ce85a315-7b00-4d04-8cdc-f1ef3536fa07" />

> *Code that measures the debugger height and width*

3. **Draw the backplate.** Measure the extents based on the height and width we need for the debugger. Crete an FCanvasTileItem, using these extents. The draw it via the Canvast Context. 

   > **Note:** Setting `BlendMode` to `Translucent` enables transparency on the debugger.

<img width="975" height="480" alt="Drawing the backplate with FCanvasTileItem" src="https://github.com/user-attachments/assets/79a859ab-5ba9-433d-9e2e-030a93d07b9c" />

> *Drawing the backplate viaa Canvas*

4. **Drawing the border of the debugger** Draw an `FCanvasBoxItem`. It is unfilled by default, which makes it perfect for our use case. 

<img width="975" height="193" alt="Drawing the border with FCanvasBoxItem" src="https://github.com/user-attachments/assets/a27f0a58-67d8-4bb1-91d8-0eb3e2309ca4" />

> *Drawing the border with an FCanvasBoxItem*

5. **Drawing the strings.** - We need to work out the location that we need to draw each string at. The X Location remains the same, but we need to add the height of the current string to the Y Location, so that the strings are drawn one after the other. This location is  then passed in as a parameter to the canvas context's `PrintAt` function, along with the string we want to render.

<img width="975" height="534" alt="Drawing strings with PrintAt" src="https://github.com/user-attachments/assets/bb88effb-aa6e-420d-be8a-19cb0a6abfc4" />

> *Drawing the text on teh debugger*

## Handling Line Wrapping

If line wrapping is disabled the debugger will adjust its width to that of the widest string. However, if line wrapping is enabled, we will handle that when processing the strings to figure out their width and height. Its roughly done in the following steps - 

1. Split the string using space as a delimiter. This means each word will be its own string in the array. This helps in preventing a line break from occuring in the middle of a word.
2. Then we accumulate words until the character limit is hit, and then we start a new string.
3. We then finally join all the combined strings with either a newline, or a newline and an indent in between them..

This is handled in `UActorRuntimeDebuggingComponent::WrapTextByWords`. 
> **Note:** This fails if a string that is one really long word is passed in

<img width="975" height="787" alt="WrapTextByWords implementation" src="https://github.com/user-attachments/assets/41070212-b584-4384-b8cb-fc8291d9fb5c" />

> *Handling line wrapping*

## Contact
Questions/Comments? Feel free to drop a line at heemgamedev@gmail.com
