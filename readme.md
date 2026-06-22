# WK Tools

Collection of tools which allow viewing and modifying game data files from Warrior Kings (Battles).

Download binaries from the [Releases page](https://github.com/AdrienTD/wktools/releases).

The tools are licensed under the MIT license.

The most interesting tools:
* __bcppack__: Create BCP files.
* __bcpview__: View/extract BCP files.
* __wkm3v__: Mesh3 and Anim3 viewer.
* __WK Mesh/Anim Import/Export addon for Blender__

Other outdated/experimental/non-essential/unsupported tools:
* __bigcpp__: Includes all gsf/cpp files into a single one.
* __obj2wkm__: Converts OBJ model files to Mesh3.
* __wkbmee__: Patch tool to access map editor in WKB.
* __wkm2obj__: Converts Mesh3 files to OBJ models.
* **md2_to_wka** and **md2_to_wkm**: Converts MD2 files to Anim3 and Mesh3 respectively.
* Tools inside the fmtinfo directory (__bcma, ltttview, wkanima, wkmtext__)
* __bcm2snr__: Converts BCM maps to SNR/TRN/PCX files.

Please take a look at README and LICENSE files for more information.

## Building

If you have Visual C++/Visual Studio/Windows SDK, open the respective command prompt, go to the directory of the tool and type "build".

The MESH3 Viewer (wkm3v) also needs D3DX_43 header and library, which you can get from the DirectX SDK June 2010.

Currently the tools only work on Windows, however some of them should also work with small modifications on other OSes.
