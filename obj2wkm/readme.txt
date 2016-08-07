wkm2obj v0.1
(C) 2016 Adrien Geets
Released under the GPL 3 (see LICENSE).
Source code in "src" directory.

This is an OBJ to MESH3 converter for Warrior Kings (Battles) modding.

Command-line usage: obj2wkm "input.obj" "output.mesh3"

Notes:
------

Before the conversion, please keep in mind that:

- The textures must be converted to TGA format.
- The texture file paths must be relative.

You can change this directly in the .MTL using a text editor. Look for lines
beginning with "map_Kd". Next to this word you should see the file name of
the texture.

If for example you have:
	map_Kd C:\mytex\lol.PNG
you should change it to:
	map_Kd lol.TGA

- You also need to copy the textures in TGA format into:
	"Warrior Kings Game Set\Textures"
  (in saved directory or in a BCP)

If your MTL file uses the texture lol.TGA you must copy it in (for example):
"C:\Program Files (x86)\Empire Interactive\Warrior Kings - Battles\
saved\Warrior Kings Game Set\Textures"


Tips:
-----

- You must cd to the OBJ file's directory, otherwise the tool might not find
  the MTL file.

- If a material doesn't have a texture, the tool will use "Gold.tga".


Known issues:
-------------

- Models converted to MESH3 might look mirrored.
