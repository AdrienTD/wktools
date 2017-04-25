bcm2snr converts a BCM map to multiple SNR, TRN and PCX files.

Usage: bcm2snr input.bcm

The SNR files will be written in the same directory as the BCM file.

BCM maps work only on WK Battles, and SNR maps work with both WK and WKB.

--- Note: ---

The path used in the input BCM file argument will be taken and written in the
SNR file to indicate where the new files used by the SNR are located in the
game data archive (data.bcp / saved directory / ...).

For example, if you type:
  bcm2snr "Maps\My Map\map.bcm"
then the new files must also be located in the "Maps\My Map" directory in the
game data archive.

Or if you type:
  bcm2snr map.bcm
then the new files must be located in the root of the game data archive.

You can also change the paths later after the conversion by opening the SNR
file in a text editor and then changing the paths of:
 * SCENARIO_TERRAIN
 * SCENARIO_HEIGHTMAP
 * SCENARIO_MINIMAP
