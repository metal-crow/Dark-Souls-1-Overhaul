# Dark Souls 1 Overhaul  
  
### Info Google doc (notes and RE'd knowledge goes here):  
https://drive.google.com/drive/folders/0BzSQv5PaltE-ci1LaDh5MDRId2M  
  
### ToDo list:  
https://trello.com/b/hay0mA7U/general-todo  
   
### Balance Changes and Suggestions:  
https://docs.google.com/document/d/1qlg4kQDUq45ufqmX8PreXchwibFwJJ3ZzK0khrHGNTY/  
    
### Collaborative IDA database (using https://github.com/DGA-MI-SSI/YaCo):  
https://gitlab.com/metal-crow/DarkSouls-IDA-db  
 * IDA with YaCo does not support attaching to a process, so to work around this, unfortunatly you have to do all your comments seperate from the debugging database. So just open 2 copies of the db (rename one "DarkSouls_local1" or whatever), and have the renamed one be the debugger.  
 * You will get some warning upon saving the IDA db like "An error happened with git" and "there is no TLS stream". It is safe to ignore them, just double check the local git log to verify that your changes did get added, then push it.  