wkm3v is a program that loads and renders a .Mesh3 file.

This program requires D3DX9_43.DLL to be installed on your computer.
If you get a message that this file is missing, then install it
from this link:
https://www.microsoft.com/en-us/download/details.aspx?displaylang=en&id=35

First you need the copy the Textures directory from "data.bcp\Warrior Kings Game Set"
to wkm3v's directory. (Use BCP Viewer to extract data.bcp.)
(You must in fact have the Textures directory in wkm3v's directory.)

To open a Mesh3 file, you can simply drag & drop the .mesh3 file into wkm3v.exe

You can also do this with an .anim3 file, but in that case wkm3v will load the
matching .mesh3 file instead.

Keys:
	e	Move up
	d	Move down
	*	Multiply scale by 1.5
	/	Divide by 1.5
	c	Change color / UV list
	Space	Enable/stop spinning