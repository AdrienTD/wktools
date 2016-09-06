WKB Map Editor Enabler v0.2
by AdrienTD


Introduction
-------------------------

wkbmee is a "trainer" which enables the Map Editor button in the main menu.
It only works on Warrior Kings Battles and not the first original one.
It NOPs some code such as the game doesn't hide the button.

It also fixes some issues in the map editor to make it usable:

- a problem in v1.1 where the editor needs the map file to be located in the
  "data" folder but will load the file from the "saved" folder instead (or from
  data.bcp).
  After running the trainer, the editor will allow files stored in the
  "saved" folder ONLY.
- a problem in v1.23 where creating a new map will fail and display an
  error message like 'texture file not found: "\map\..."'
  The file path should begin with "Map\" and not "\map\".

Tested on WKB version 1.1 and 1.23.
I haven't tested the trainer on WKB version 1.0 yet.


Instructions
-------------------------

0) Be sure the file name of the game executable is "Warrior_Kings_Battles.exe"
   or "Warrior Kings - Battles.exe".
   If it's different, rename it to the correct file name.
   I recommend you to run the game in a window. To do this, run the Registry
   Editor, find the "...\Black Cactus\Warrior Kings Battles\Renderer" key,
   add in this key a new DWORD value named "fullscreen" and set this value to
   0. Or if you don't know how to use the Registry Editor, take a look at the
   "WK Registery Configs" directory in the modding kit.

1) Run the "wkbmee.exe" file.
   A dialog box appears. Do NOT click on "OK", you must leave the message as is.

2) Run the game.

3) While WKB is running, go back to the message box and NOW you can click on OK.
   Tip: Click on OK while WKB is playing the introduction video or showing the
        disclaimer screen to make the Map Editor button appear immediately.

4) Wait until a second message appears.

5) Click on OK.

6) Now you can see the Map Editor button on the bottom of the main menu.
   If you don't see it, play a map (or play the interactive tutorial),
   then go back to the main menu.

7) Click on the Map Editor button.

8) Enjoy! :)


Map editor quick start
-------------------------

The map editor has a menu bar.
When you run it for the first time, you can open and create a map.

When you want to create a new map, you need to specify a name.
In version 1.1, you also need to open a texture database.
Create an empty file "Map\Map_Textures\All_Textures.dat" in the "saved" folder.
If you load it as the texture database, the game will load from data.bcp
instead.

In v1.1 you can only open files in the "saved" directory in WKB's directory.
In v1.23 you don't have this restriction (it's fixed by the game's developers).

After opening/creating a map, press on SPACE to toggle between 2D/3D view.
If it doesn't work, click on the right view panel then press on SPACE.
Use PageUp/PageDown to move up/down.
In v1.1, move the mouse while pressing on the right mouse button to rotate the
camera.


It doesn't work!
-------------------------

If you don't get the map editor after following the instructions, be sure that:

- the game executable's file name is "Warrior_Kings_Battles.exe" or
  "Warrior Kings - Battles.exe".
- you click the OK button in the first message box when the game is already
  running.
- you use WKB version 1.1 or 1.23. If not you can download a patch.


Changelog
-------------------------

v0.1 (20/12/2015): First release.
v0.2 (06/09/2016): Now also looks for processes named "Warrior Kings - Battles.exe".
