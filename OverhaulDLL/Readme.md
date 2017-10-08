# Dark Souls Overhaul DLL Build  

The Overhaul mod DLL is a plugin for [this Direct3D9 overlay](https://github.com/SeanPesce/Direct3D9-Overlay).  

## Developer Info:  

* **Compiling:** To compile the project, you will need Microsoft's **[June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)**. I've added the libraries and headers to our project directory (in the `/lib/DirectX/` folder) in hopes that everyone won't have to download/install the SDK, but if that doesn't work you'll have to install it (and add it to the include path if you install it to a non-default location).  Additionally, make sure to use the **[x86 Release build configuration](http://i.imgur.com/3FfTwYQ.png)** in Visual Studio, otherwise you'll probably run into linker errors. The compiled **DS_Overhaul.dll** file can be found in the `/Release/` folder.  

* **Initialization:** One-time initialization code should be implemented in one of three functions, all called exactly once from `DllMain()` when the mod DLL is first loaded. These functions are listed below and can be found in the file **DarkSoulsOverhaulMod.cpp**:  
  * **`on_process_attach()`:** This is the earliest function where our code will be executed. The function is called from the same thread as `DllMain()`, delaying game loading until the function is finished executing, so any code implemented here should be limited to tasks that absolutely MUST be executed before the game loads.  
  * **`on_process_attach_async()`:** This function executes in a separate thread from `DllMain()`, so it doesn't halt the loading of other libraries. Code implemented here should consist of initialization tasks that don't need to be performed before a specific event (or in a specific order).  
  * **`initialize_plugin()`:** This function is called a bit later than `on_process_attach`. More specifically, this function is called after the DirectX9 overlay has been initialized, allowing you to print messages to the overlay text feed and/or in-game console.  

* **Other important functions:** *@TODO: Explain the various functions that allow us to run code during important events such as rendering the frame, handling player input, etc.*  
  
* **Printing overlay messages:** Printing messages to the overlay is useful for real-time debugging and developer/end-user feedback. To print a message to the overlay, use the **`print()`** function. To print a message to the in-game console, use the **`print_console()`** function. For more information about these functions, see the documentation in **PluginImports.h**, where both functions are defined.  

* **Using the in-game console:** *@TODO: Directions for registering new console commands, creating aliases, creating console scripts, etc.*  

* **Code injection, memory writes, & multi-level pointers:** If you have questions about any of these topics, contact @SeanP or @Ashley on Discord. *@TODO: Explain these topics.*  
 
 
 __________
 
 
 Any questions/comments/suggestions about the DLL code should be directed to @SeanP, @Ashley, or @metal-crow on the project Discord server.  
 
