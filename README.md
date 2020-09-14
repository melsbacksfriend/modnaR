# modnaR
A Homebrew app to apply a random mod for a Switch game

## Correct folder structure: 

```
sdmc:
|__modnaR
|  |__(GAME NAME)
|     |__(SOME NUMBER)
|        |__(GAME TID)
|           |__(MOD FILES)
|
|__modcount.txt

```

Example: 

```
sdmc:
|__modnaR
|  |__Pokemon Sword
|     |__1
|        |__0100ABF008968000
|           |__romfs
|           |  |__bin
|           |     |__archive
|           |        |__field
|           |           |__resident
|           |              |__data_table.gfpak
|           |
|           |__fsmitm.flag
|
|__modcount.txt

```

## Usage: 

1. Create a document on the root of your sd card called modcount.txt with the number of mods.  
2. Launch modnaR. 
3. Press A. 
4. Type in the name of the game EXACTLY HOW YOU NAMED THE FOLDER WITHIN THE MODNAR FOLDER!  
5. Wait till it says done.  
6. You're done!  

## Origin of the program's name: 

Random spelled backwards.  

## Credits: 

CompSciOrBust for N-Xplorer's copy function.  
