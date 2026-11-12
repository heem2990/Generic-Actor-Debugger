# Actor Runtime Debugging Tool

What does this do?
It’s a debugger that can print any debug info, in form of a string, on an actor. Its highly customizable, and the idea is for any actor type, you should be able to add relevant strings to show on top of that actor. 
<img width="1081" height="581" alt="Screenshot 2026-11-12 094210" src="https://github.com/user-attachments/assets/bd9f21bc-94b7-446f-8270-68f42a7c6f98" />

Simple, quick and easily extensible actor debugger 
What does this tool do? It’s a debugger that can print any debug info, in form of a string, on an actor. Its highly customizable, and the idea is for any actor type, you should be able to add relevant strings to show on top of that actor. 
 
Shows the same debugger on 3 different actors, and has a dynamic string example on one of the characters that gets added when it enters a trigger volume.

Why – Unreal already has a bunch of really neat debuggers already like the gameplay debgugger, visual logger, rewind debugger, and they are all pretty awesome, and incredibly helpful in different situations. 
However, none of them are things you can bring up at run time, on multiple different actors at the same time. 
Additionally, adding information to gameplay debugger basically requires you to create a new debugger, and that can be a bit of setup when all you really want to do is see a state string of an actor. 
Finally, you could just use “GEngine->DrawOnScreenDebugMessages”, and that works really well, however, since it draws it all in screen space, its not always simple to easily associate with the actor that’s printing the string. 
This tutorial is divided in two parts. The first part shows you how you can take this debugger as is, and adapt it to your project per your needs, and how you can extend it. 
The second part of the tutorial talks about the process of making this debugger. 
How do I use this debugger, and how do I set it up for different types of Blueprint of Native actors for the project?
1.	This debugger basically has 2 parts to it. 
a.	The Debugger Component – This is the component that does the bulk of the work, and is responsible for collecting and drawing the data on the debugger. You’ll also be able to to maniupulate the appearance and options for the debugger via this component. 
b.	The Debugger Interface – This is basically a way to interact with the component, to get the component from an actor to enable or disable it, or to add any dynamic strings from outside the actor. 
2.	The first thing you want to do is add the “ActorRuntimeDebuggingComponent” on the actor that you want to debug. You can add this component either in Blueprint, OR, natively. You can quickly test at this point. On the component enable the option “Begin Enabled”, then when you start PIE the actor should draw a debugger (if not, make sure the cvar “ActorDebugger.Enable” is enabled. This is the main gate to enable/disable this debugger)
3.	Then, the actor needs to implement the debugger interface “ActorRuntimeDebuggerInterface”. This can be done in Blueprint (via. Class settings), or natively.  This interface exposes functions that let you get the debugger component, or implement the “CollectData” function on it, that lets you add whatever debug strings you want to the debugger. 
Note: You can colorize strings by using “{color}” tags in your string, everything after that color tag will be that color. You can find a list of available colors in “ColorList.h”, theres like a 100 different ones 
<img width="975" height="279" alt="image" src="https://github.com/user-attachments/assets/1f247635-0c79-4a54-9d5c-fe41eaec07a2" />

Example of Component added to an actor in Blueprint
<img width="975" height="420" alt="image" src="https://github.com/user-attachments/assets/21f7b548-48b0-4759-b1f8-eed9e037b5cd" />
Example of Component added to an actor in C++

4.	Once you have implemented the interface on the actor, you should be able to override the function “Collect Data”, on the actor that will allow you to add strings to an array of strings. This array of strings is what we show on the debugger at run time. 
a.	NOTE: When implementing “Collect Data” in  blueprint, make sure to make a call to the Parent “Collect Data”, that way if that function is also implemented natively, both the native and BP version of the function will properly get called. 

 <img width="975" height="506" alt="image" src="https://github.com/user-attachments/assets/b8a0afde-b464-4c08-bb06-c1d083682ad8" />

Example of adding debug string in Blueprint (can see the output ON the manny character)
 <img width="975" height="414" alt="image" src="https://github.com/user-attachments/assets/a124ce40-c5f9-4932-ade7-8674d2769701" />

Example of adding debug string natively. Can see this output on manny character
5.	Now, if you wanted to add debug strings that are added via some external impetus. E.g. the actor moves into the range of an interactable. You can make use of the delegate “Collect Data Delegate” that the component exposes. This delegate gets called when the component is collecting data, and you can add strings to the component directly. 
 <img width="975" height="492" alt="image" src="https://github.com/user-attachments/assets/892b7d77-38c3-410f-b82f-0251d9e3b8d3" />

Example of a Trigger Volume adding a dynamic string whenever an actor starts overlapping it.
6.	In case you have information that you want to print for ALL actors that implement this component, you can do so in the components “CollectData” function itself. 
 <img width="975" height="567" alt="image" src="https://github.com/user-attachments/assets/94823526-7ea6-45e8-94b4-045ae864f090" />


7.	You might want to put consideration into how you want to toggle the visibility of this debugger. You have a couple of options on that front 
a.	On the component itself, you can enable the option “BeginEnabled”. If set, whenever the actor begins play, we will automatically also start showing the debugger. 
b.	Alternatively, you can make it be more dynamic, and make use of the Blueprint exposed function “Toggle if Debugger Present” to enable or disable the debugger. You can attach this to a controller input to make it easier or console command or some other input, this can be done natively or in blueprint.
 <img width="975" height="346" alt="image" src="https://github.com/user-attachments/assets/8a4d0984-d5a2-4270-98ef-1791438894da" />

Example of toggling debugging dynamically 
8.	Finally, you might want to consider the cosmetic options, basically how do you want the debugger to be drawn. This can be especially handy if you are enabling debuggers for multiple different types of actors, as simple color changes can help you quickly identify what actor the debugger is for. The options available are – 
a.	Collect Data Frequency Type – Whether to collect data on Timer or on Tick
i.	Collect Data Frequency Seconds – If on timer, how frequently to collect data in seconds
b.	Half Vertical Padding – Any padding to add at top/bottom of debugger before drawing text
c.	Half Horizontal Padding – Any padding to add to left/right of debugger before drawing text
d.	Border Thickness – How thick should the border thickness be (0.0 is no border)
e.	Line Warp Handling – How should debugger handle long string
i.	No Warping – The debugger will print strings as is, and will adjust its size to the widest string.  
ii.	Warp With Line Breaks Only – The debugger will warp the lines if they exceed max characters, and will add a line break when warping
iii.	Warp With Line Breaks and Indent - The debugger will warp the lines if they exceed max characters, and will add a line break and an Indent when warping
f.	Max Width – the max width of debugger (relevant if line warping)
g.	Max Characters Allowed Per Line – how many characters to allow before warping (relevant if line warping)
h.	Backplate Color – What is the color of the backplate of the debugger (Supports translucency)
i.	Border Color – What is the border color of the debugger
j.	Debugger Offset – We draw the debugger at this offset from the actors world location (projected onto the screen). So if you want this on top of a character, you’d add the appropriate value to Z. 
k.	Begin Enabled – If set, this debugger will enable itself on Begin Play
l.	Add To Visual Logger – if Enabled, will add the text to visual logger as well
 <img width="975" height="734" alt="image" src="https://github.com/user-attachments/assets/ee13534a-fb54-4f74-98fc-4361b006b807" />



How do I create a debugger like this?
A debugger basically has 2 different parts to it – 
1.	How does it get the data it prints? There are many ways to do this. Gameplay Debugger is setup so that you for every debugger type, you have to implement the “CollectData” function, and each debugger is expected to implement this. Visual Logger on the other hand lets you log from anywhere using UE_VLOG functions, and then it collates the debug information based on the actor it was logged on, and the channel. The Actor Debugger was modelled after the gameplay debugger, however, it only takes strings as input, as that is the only thing we support printing on screen, for this tool. 
2.	How does it print on screen the data it has? This can also have many different forms, and will depend on the type of debugger you want to make. It could be something like the GameplayDebugger which exposes a canvas that lets you draw (text/shapes) in either world space or screen space. It could be something like visual logger, that requires a more bespoke editor.  For the Actor Debugger, since its modelled after gameplay debugger, it also exposes a canvas that we then use to draw the debugger. 
Specifically for Actor Debugger – 
Collecting Data –
-	We wanted the ability for any type of actor to be able to quickly add this debugger, and start printing to it. Making it an ActorComponent provides that flexibility, however, we still need the actor itself to easily add the strings it needs when collecting Data. 
-	Using an interface that the actors implement made sense to me for a few reasons
o	It can be implemented in Blueprints, Native, or, both. Which is good flexibility to have, so tech designers can then easily add this debugger to any new blueprint actor they could be making, without having to involve engineering. Also allowing engineering to add stuff natively as they create new systems. 
o	The debugger component can now be agnostic of what type of actor is implementing the interface. Additionally, any other systems that needs to interact with this debugger, can now just solely do so via the interface functions, and doesn’t need knowledge of what type of actor is implementing this interface. 
-	In addition to an interface, the component has a delegate, that gets broadcasted at the end of the “CollectData” function. This allows different systems to easily get callbacks, when data is being collected, and thus easily push data to this debugger so it can be printed. So for example, any other components that the actor might have, would just need to get the debugger component from the owner, and subscribe to this delegate. Now it will get a “Collect Data” delegate that it can use to push debug messages about its own state. 
-	Finally, the component itself currently adds a couple of very generic messages, one being the actor name, and second being its location. Any project that might need to expose more generic information like this for ANY actors can just simply do so in the ActorRuntimeDebuggingComponent::CollectData function. 
 <img width="975" height="600" alt="image" src="https://github.com/user-attachments/assets/a9542568-ffdb-46d5-91eb-b2a6ee0d8d54" />

-	When does Collect Data get called? The plugin by default provides you 2 different options. It can collect data on component tick, or on a timer. However, the idea here is you just need to call “ActorRuntimeDebuggingComponent::CollectData” , and you can extend the plugin to call this from wherever appropriate for your project.
Drawing Data
-	The first thing here is that we need a callback, when the game is draw, and a canvas, that we can draw two. Both of these things are provided by UDebugDrawService. This is basically what the gameplay debugger uses to register irs draw call. 
-	The ActorRuntimeDebuggerComponent registers itself to UDebugDrawService, and gets the ::DrawCallback function, this gets called with a PlayerController and a Canvas. We do this registering, and unregistering when the debugger is toggled on or off. 
 <img width="975" height="581" alt="image" src="https://github.com/user-attachments/assets/22c4b93d-1042-4be4-9c7c-fa70ef9fab52" />

-	The Actor Runtime Debugging Component wraps this UCanvas in an FGameplayDebuggerCanvasContext. The reason for doing this is mainly that the gameplay debugger canvas context provides a bunch of neat wrapper functions around the draw functions in UCanvas, that make drawing text a bit easier. Additioonally, the coloring of text is handled via FGameplayDebuggerCanvasContext, so that isn’t available via UCanvas directly. 
 <img width="975" height="115" alt="image" src="https://github.com/user-attachments/assets/2a39c069-7b77-4b61-a276-3865c821ca31" />

-	This FGameplayDebuggerCanvasContext is then what we use to draw the debugger. 
-	The first thing we do is determine where to draw, we get the “WorldLocation”, then use the canvas context, to project it to screen space location. If the World Location isn’t visible on screen, it won’t get drawn. 
-	Then, we measure how big our debugger needs to be. The GameplayDebuggerCanvasContext provides helper function to measure a strings length/width, which is what we leverage to determine the height/width of our debugger box.
 <img width="975" height="522" alt="image" src="https://github.com/user-attachments/assets/ce85a315-7b00-4d04-8cdc-f1ef3536fa07" />

-	 Once you have the extents, you can pass that in to draw an FCanvasTileItem, that can then be drawn by the Canvas Context. 
o	Note: Setting BlendMode to Translucent lets you have transparency on the debugger. 
  <img width="975" height="480" alt="image" src="https://github.com/user-attachments/assets/79a859ab-5ba9-433d-9e2e-030a93d07b9c" />

-	To draw the border, you can jut draw an FCanvasBoxItem. The default is not filled, so it works as a good substitute for the border. 
 <img width="975" height="193" alt="image" src="https://github.com/user-attachments/assets/a27f0a58-67d8-4bb1-91d8-0eb3e2309ca4" />

-	Finally, to draw the string, we figure out the location we need to draw it at, and then make use of the PrintAt function of the canvas context to draw the string. 
 <img width="975" height="534" alt="image" src="https://github.com/user-attachments/assets/bb88effb-aa6e-420d-be8a-19cb0a6abfc4" />

Handling Word Warping – when processing the strings for figuring out its width and height, we measure if the string has more characters than allowed, if so, we first separate the string into an array of words, this is to prevent a newline in middle of the word. Then, we basically accumulate words, until we hit the character limit, then we start a new string. Finally, we join all the strings with a newline, or with a newline and indent. This is handled in UActorRuntimeDebuggingComponent::WrapTextByWords 
  <img width="975" height="787" alt="image" src="https://github.com/user-attachments/assets/41070212-b584-4384-b8cb-fc8291d9fb5c" />



