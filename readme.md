# WK Tools source code

This is the source code of some tools which allow viewing and modifying game data files from Warrior Kings (Battles).

If you want to use these tools you might want the binaries of the tools. You can download them from:
* [my Google Drive](https://drive.google.com/open?id=0B-xteMV6gdTeWG9rSHNmS19XQWs)
* [Warrior Kings Events site](http://warriorkingsevents.jimdo.com/media)

These tools are licensed under the MIT license:
* __bcppack__: Create BCP files.
* __bcpview__: View/extract BCP files.
* __bigcpp__: Includes all gsf/cpp files into a single one.
* __obj2wkm__: Converts OBJ model files to Mesh3.
* __wkbmee__: Patch tool to access map editor in WKB.
* __wkm2obj__: Converts Mesh3 files to OBJ models.
* __wkm3v__: Mesh3 and Anim3 viewer.
* **md2_to_wka** and **md2_to_wkm**: Converts MD2 files to Anim3 and Mesh3 respectively.
* Tools inside the fmtinfo directory (__bcma, ltttview, wkanima, wkmtext__)
* __bcm2snr__: Converts BCM maps to SNR/TRN/PCX files.

__UPDATE:__ Tools previously released under the GPL3 license are now relicensed to the MIT license.

Please take a look at README and LICENSE files for more information.

## Compilation

If you have Visual C++/Visual Studio/Windows SDK, open the respective command prompt, go to the directory of the tool and type "build".

The MESH3 Viewer (wkm3v) also needs D3DX_43 header and library, which you can get from the DirectX SDK June 2010.

Currently the tools only work on Windows, however some of them should also work with small modifications on other OSes.
