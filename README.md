# Dark Souls 1 PvP Fixes and Overhaul  
  
### Info Google doc (notes and RE'd knowledge goes here):  
https://drive.google.com/drive/folders/0BzSQv5PaltE-ci1LaDh5MDRId2M  
  
### ToDo list:  
https://trello.com/b/hay0mA7U/general-todo  
   
### Balance Changes and Suggestions:  
https://docs.google.com/document/d/1qlg4kQDUq45ufqmX8PreXchwibFwJJ3ZzK0khrHGNTY/  

### Collaborative Ghidra database
https://github.com/soulsmods/Soulsborne-Modding-Wiki/wiki/Ghidra-Shared-Repo

### Custom Archive files (new SFX, Effects, etc):
https://gitlab.com/metal-crow/darksoulsremastered-overhaul-archives

### Requirments
This requires the [Microsoft Visual C++ Redistributable for Visual Studio 2015-2019 ](https://aka.ms/vs/16/release/vc_redist.x64.exe)

Building from source requires Visual Studio 2017, and access to some private repos (ask metalcrow or SeanP)  


## Description
The mod is completely backwards compatible with vanilla dark souls 1 by default. In __legacy__ mode, it includes a number of significant but fully backwards compatible bugfixes, quality-of-life improvements, visual glitch fixes, and functional anti-cheat measures.  
It also unlocks a number of in-engine limitations and fixes other engine problems (including disabling the terrible built-in anti-cheat), making it perfect to extend further with other mods.  

If you so wish, you can enable __overhaul__ mode at any time to get the full suite of improvements this mod gives.  
This will __not__ separate you from vanilla players. A player will have the option of deciding if they want to enable overhaul mode once they receive a connection request from a player with it enabled, and visa-versa. Preferred defaults and auto-allow/decline options can be configured.  
Enabling Overhaul mode by default is highly recommended, since it dramatically improves pvp experience on a number of metrics (weapon viability, magic use, and more) while still keeping the fundamental dark souls 1 pvp experience intact.  
  
## Changelist
	
### Legacy

* NPCs can no longer be killed by hackers

* Hackers can no longer force you into Binoculars or Dragon form, or apply other bad effects to you

* Tranquil Walk of Death bug fixed

* A bug that allows hackers to crash your game has been fixed

* Backstabs will no longer teleport you if you're unreasonably far away (both anti-cheat and QOL)
  
* Bug fix for invisible attacks after backstabs

* Dead Angles and Ghost Hits are back, and made slighly more consistent  

* Equipment (except weapons) can again be changed while in any animation

* Bug with HP bar not reflecting the correct amount of health has been fixed (but can be disabled)

* The game has increased memory capacity (larger files can be loaded)

* You can now invade or summon phantoms __anywhere__.  
This solution is imperfect, as areas where you could not do multiplayer before but are right next to areas you could (I.E Undead Burg bonfire), are treated as seperate multiplayer areas.  
(Note that DSR allows summons even with the boss dead).  

* Support for unlimited save slots.  
Press left/right arrow or left/right dpad while in the Load Game menu to change your current save.  
It's like pages in a book. Each "page" is 10 characters.  

* Support for custom archive file loading  
Specify folder location + filenames to use, and the alternate path will be used to load them

* Fix to prevent homing spells from desyncing (only if both players have mod)

* Fix the "broken ankles" bug when rolling

* (Tentatively) compatable with [DSR Watchdog](https://www.nexusmods.com/darksoulsremastered/mods/160). Rename the d3d11.dll file given by watchdog to watchdog.dll, put it in the same folder as DarkSoulsRemastered.exe, and in this mod's d3d11_mod.ini, set `d3d11Chain=watchdog.dll`  
This will be obsolete eventually

* Crash handler and reporter  
Hope to obsolete this when i make compatable with ModEngine2

* Option to use steam display names instead of character names

* Option to disable the durability system

* Steam netcode updated to reduce latency and hide IP address

* New item, "Searching Red Eye Orb" (id 104), that searches all multiplayer areas while trying to invade  
This allows you to invade any area across the entire game via 1 single use. It will continually search all areas rapidly, until cancelled or invasion found

### Overhaul

* Gestures can be canceled via rolling

* Animations:
  * Firestorm startup and main animation have been sped up (x10 and x1.6)
  * Gravelord Sword Dance startup and main animation have been sped up (x10 and x1.6)
  * All kneeling heals have had their startup sped up (x6)
  * Lightning Spear startup and main animation have been sped up (x10 and x1.2)

* All whiff animations on weapons have been removed (they always have the on-hit animation instead)

* BloodBorne rally system has been implemented as a replacement for the Occult upgrade path  
HP recovery is equal to `(0.05 + (upgrade level of weapon / 10.0)) * damage given`  
Time to recover the hp is 5000 ms, or on weapon toggle.  

* Some spell types no longer lock your movement or rotation while they are being casted  
You can freely walk, run, and rotate as you are casting them, and pivot cancels work as normal    
These include everything EXCEPT: Combustions, Firestorms, and all miracles but Lighting Spear and Darkmoon Blade/Sunlight Blade  

* Fix to prevent curved swords from stunlocking

* Fix for the halberd instant running attack glitch in mud and pvp

* Backstab distance and angle has been reduced. (60 degrees to 45, 1.5 distance to 1)

* Large scale health increase across the board.  
Average SL 125 build should be ~3k HP.