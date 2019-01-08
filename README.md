# Dark Souls 1 Overhaul  
  
### Info Google doc (notes and RE'd knowledge goes here):  
https://drive.google.com/drive/folders/0BzSQv5PaltE-ci1LaDh5MDRId2M  
  
### ToDo list:  
https://trello.com/b/hay0mA7U/general-todo  
   
### Balance Changes and Suggestions:  
https://docs.google.com/document/d/1qlg4kQDUq45ufqmX8PreXchwibFwJJ3ZzK0khrHGNTY/  
    
### Collaborative IDA database (using https://github.com/DGA-MI-SSI/YaCo v2.6):  
https://gitlab.com/metal-crow/darksoulsremastered-ida-db

## Changelist

* NPCs and Bosses (kindof, WIP) can no longer be killed by hackers
* Backstabs will no longer teleport you if you're unreasonably far away (both anti-cheat and QOL)
	
* Gestures can be canceled via rolling

* Animations:
  * Firestorm startup and main animation have been sped up (x10 and x1.6)
  * Gravelord Sword Dance startup and main animation have been sped up (x10 and x1.6)
  * All kneeling heals have had their startup sped up (x6)
  * Lightning Spear startup and main animation have been sped up (x10 and x1.2)
  * Backstab animations are sped up (after 3 seconds, x1.25)

* All whiff animations on weapons have been removed (they always have the on-hit animation instead)

* BloodBorne rally system has been implemented as a replacement for the Occult upgrade path  
HP recovery is equal to (0.05 + (upgrade level of weapon / 10.0)) * damage given  
Time to recover the hp is 5000 ms, or on weapon toggle.  

* Dead Angles and Ghost Hits are back

* Equipment (armor and rings, but not weapons) can again be changed while in any animation

* Bug with HP bar not reflecting the correct amount of health has been fixed

* The game has increased memory capacity (larger files can be loaded)

* Max sized effect ids can now be sent over the network. This means any added effect ids are visable over multiplayer

* Some spell types no longer lock your movement or rotation while they are being casted  
You can freely walk, run, and rotate as you are casting them. Also, you can roll cancel them during their windup  
These include everything EXCEPT: Combustions, Firestorms, and all miracles but Lighting Spear and Darkmoon Blade/Sunlight Blade  

* You can now invade or summon phantoms __anywhere__, and can still do so after bosses are killed  
This solution is imperfect, as areas where you could not do multiplayer before but are right next to areas you could (I.E Undead Burg bonfire), are treated as seperate multiplayer areas.