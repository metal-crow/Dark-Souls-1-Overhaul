# Dark Souls Overhaul DLL Build  

The DLL is a plugin for [this Direct3D9 overlay](https://github.com/SeanPesce/Direct3D9-Overlay).  

## Developer Info:  

* **Compiling:** To compile the project, you will need to install the **[June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)** (and add it to the include path if you install it to a non-default location).  Additionally, make sure to use the **[x86 Release build configuration](http://i.imgur.com/3FfTwYQ.png)** in Visual Studio, otherwise you'll probably run into linker errors. The compiled DS_Overhaul.dll file can be found in the /Release/ folder.  

* **Initialization:** One-time initialization code should be implemented in one of two functions, both found in the file **DarkSoulsOverhaul.cpp**:  
  * **`on_process_attach()`:** This is the earliest function where mod code will be executed. It is called from **DllMain()** when the mod DLL is first loaded.  
  * **`initialize_plugin()`:** This code is also run one time after the DLL is loaded, but a bit later than `on_process_attach`. Unlike `on_process_attach`, this function is executed in a separate thread from the main game-loading thread, allowing the game to load simultaneously. For this reason, slow tasks and/or one-time AoB scans should probably be performed here, to ensure the data we're scanning for has been loaded into memory.  

* **Printing overlay messages:** Printing messages to the overlay is useful for real-time debugging and developer/end-user feedback. To print a message to the overlay, use the **`_PRINT_OVERLAY_()`** preprocessor macro. The signature of the related function can be found below. The arguments are as follows:    
  * `message` is the message to be printed.
  * `duration` is the lifespan of the message (how long it will stay on-screen), in milliseconds.
  * `include_timestamp`, if true, will prepend the current 24-hour timestamp to the printed message.  

```c++
void SpD3D9OTextFeed::print(const char *message, unsigned long long duration, bool include_timestamp)
```

* **Code injection, memory writes, & multi-level pointers:** If you have questions about any of these topics, contact @SeanP or @Ashley on Discord. I'll probably write up some notes about these later, if need be.  

* **Bugs:** Visual Studio will probably highlight member functions of the `SpPointer` class, claiming the class doesn't contain the function. I'm not sure why this is happening, but it's a false error, and the project will compile fine (assuming everything else is set up correctly).  
 
 
 __________
 
 
 Any questions/comments/suggestions about the DLL code should be directed to @SeanP, @Ashley, or @metal-crow on the project Discord server.  
 
