These are two executables that allows you to put MD2 models (from Quake 2
and also other games) into Warrior Kings (Battles).

The first executable "md2_to_wkm.exe" creates a .mesh3 file whereas the
second executable "md2_to_wka.exe" creates an .anim3 file.

If you want a static model, you only need a mesh3 file. But if you want
an animated model, you need both a mesh3 file '''AND''' an anim3 file.

There is also the batch file "conv.bat" which makes the conversion much easier.
All you need to do is:

* Put your MD2 file inside this directory (with conv.bat and the exes)
* Open a command-prompt and go to this directory.
* Type: conv "Name of MD2 file without extension"
  For example, if the model file is named "my model.md2":
        conv "my model"

You will see a new directory named "my model" with a mesh3 file and various
anim3 files.

You also need to convert the texture of your model into the TGA format.
The name should be the same as the MD2 file (for example "my model.tga").
Then you put the file into "Warrior Kings Game Set/Textures" in the game data.

AdrienTD