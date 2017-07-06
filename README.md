# Dark Souls 1 Overhaul  
  
### Info Google doc (notes and RE'd knowledge goes here):  
https://drive.google.com/drive/folders/0BzSQv5PaltE-ci1LaDh5MDRId2M  
  
### ToDo list:  
https://trello.com/b/hay0mA7U/general-todo  
   
### Balance Changes and Suggestions:  
https://docs.google.com/document/d/1qlg4kQDUq45ufqmX8PreXchwibFwJJ3ZzK0khrHGNTY/  
    
### Collaborative IDA database (using https://github.com/DGA-MI-SSI/YaCo):  
https://gitlab.com/metal-crow/DarkSouls-IDA-db  
Note: It is very important you do not "Pack (Deflate)" the IDA database when you close it (use "Pack (Store)" instead), as this compresses the database and git can no longer use differential compression.  
Additionally: IDA with YaCo does not support attaching to a process, so to work around this; rename your DarkSouls_local.idb to DarkSouls_local1.idb (or whatever). Open that, do your debugging and commenting. Once finished, close, rename back to DarkSouls_local.idb, and commit your changes.  

### Resources:  
010 Editor Templates: https://www.dropbox.com/s/w7nqguyshn42wi2/Dark%20Souls%20Templates%20v2.zip?dl=1  
Param dumps: https://drive.google.com/drive/folders/0B_Und9n_kD1vN3lxelFJSnExbEk  
Cheat Engine tables: https://github.com/saeshyl/reCode/tree/master/Dark%20Souls (DARKSOULS.ct, Phonkz.ct, Technojacker.ct)  
File formats and archives: https://github.com/Danilodum/dark_souls_hkx/  