# Dark Souls 1 PvP Fixes and Overhaul  
  
### Info Google doc (notes and RE'd knowledge goes here):  
https://drive.google.com/drive/folders/0BzSQv5PaltE-ci1LaDh5MDRId2M?resourcekey=0-9E8yBDVHw0gzKEuWYvCdEA&usp=sharing
  
### ToDo list:  
https://trello.com/b/hay0mA7U/general-todo  
  
### Custom Archive files (new SFX, Effects, etc):
https://gitlab.com/metal-crow/darksoulsremastered-overhaul-archives

### Nexus Mods link
https://www.nexusmods.com/darksoulsremastered/mods/466

### Discord
https://discord.gg/nABqj8G

### Installation
This requires the [Microsoft Visual C++ Redistributable for Visual Studio 2015-2019 ](https://aka.ms/vs/16/release/vc_redist.x64.exe). You should already have it, but just in case.  
  
Unzip the contents of the zip into your DARK SOULS REMASTERED folder to install (so d3d11.dll should be in the same folder as the game exe). If a command window pops up on game start, the mod is installed.  
  
### Uninstall

To uninstall or disable the mod, remove d3d11.dll. I also recommend deleting or restoring your old save on uninstall, to prevent a possible ban now that the mod is disabled.  

## Description
The mod is completely backwards compatible with vanilla dark souls 1 by default. It will __not__ separate you from vanilla players.  
  - As a host, anyone connecting with you will conform to your mod's mode. 
  	- I.e, if you are in overhaul mode and they are in legacy, they will either change to overhaul or disconnect.
  	- If it's a non-mod user who is connecting, you can configure if you want to deny their connection attempt, or have your own mode change to match them (the default) via F4.
    - Of note: if you have an existing mod-user connected and a non-mod user connects, you will have a few seconds of packet loss while you and the other mod user fall back to the older steam packet API.
  - As a guest, you can configure who you want to be able to connect to. By default you accept all connections and change your mode to match the host.
  	- You can toggle this behavior with F1 (to disallow connecting to non-mod hosts), F2 (to disallow connecting to legacy mode hosts), and F3 (to disallow connecting to overhaul mode hosts).

Modes can be switched between by pressing F5.

__Legacy__ mode is the default starting mode the mod is in (this can be changed in the ini). It includes a number of significant but fully backwards compatible bugfixes, quality-of-life improvements, visual glitch fixes, and functional anti-cheat measures.  
It also unlocks a number of in-engine limitations and fixes other engine problems (including disabling the terrible built-in anti-cheat).  

__Overhaul__ mode can be switched on to get the full suite of improvements this mod gives. It dramatically improves pvp experience on a number of metrics (weapon viability, magic use, and more) while still keeping the fundamental dark souls 1 pvp experience intact.  
  
  
## Changelist
	
### Legacy

* NPCs can no longer be killed by hackers

* Hackers can no longer force you into Binoculars or Dragon form, or apply other bad effects to you

* Tranquil Walk of Death bug fixed

* A bug that allows hackers to crash your game has been fixed

* Backstabs will no longer teleport you if you're unreasonably far away (both anti-cheat and QOL)
  
* Bug fix for invisible attacks after backstabs

* Dead Angles and Ghost Hits are back, and made slighly more consistent  
This only affects you, the client. If you are playing with a non-mod user, they can dead angle you but you can't them.

* Equipment (except weapons) can again be changed while in any animation

* Bug with HP bar not reflecting the correct amount of health has been fixed (but can be disabled)

* The game has increased memory capacity (larger files can be loaded)

* You can now summon phantoms anywhere.   
This solution is imperfect, as areas where you could not do multiplayer before but are right next to areas you could (I.E Undead Burg bonfire), are treated as seperate multiplayer areas.  
(Note that DSR allows summons even with the boss dead).  

* Support for unlimited save slots.  
Press left/right arrow or left/right dpad while in the Load Game menu to change your current save.  
It's like pages in a book. Each "page" is 10 characters.  

* Support for custom archive file loading  
Specify folder location + filenames to use, and the alternate path will be used to load them

* Fix to prevent homing spells from desyncing (only if both players have mod)

* Fix the "broken ankles" bug when rolling

* Crash handler and reporter  
Hope to obsolete this when i make compatable with ModEngine2

* Option to use steam display names instead of character names

* Option to disable the durability system

* Steam netcode updated to reduce latency and hide IP address (only if both players have mod)

* Fix for the halberd instant running attack glitch in mud and pvp

* New item, "Searching Eye Orb" (id 104, available from the Undead Merchant), that searches all multiplayer areas while trying to invade  
This allows you to invade any area across the entire game via 1 single use. It will continually search all areas rapidly, until cancelled or invasion found

* New item, "Unbound Eye Orb" (id 105, available from the Undead Merchant), that does PTDE style red eye invasions: searching infinitely upwards in Soul Level.  
This means, as a SL 100 character, you can invade anyone from SL 90 to SL 713.  
This doesn't also enable infinite upwards weapon level searching, however.  

* New item, "Twin Eye Orb" (id 109, available from the Undead Merchant), that does both of the above orbs' functions, combined.  

* Basic update check system

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

* Large scale health increase across the board.  
Average SL 125 build should be \~3k HP.
