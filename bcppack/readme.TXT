bcppack - BCP packer
(C) 2016 Adrien Geets
Released under the terms of the GPL3. See LICENSE for more information.

Usage: bcppack "path/to/directory/to/pack" "output.bcp" "version"
Version can be either 0 for WK 1.0-1.3, or 2 for WK 1.4 and WKBattles
For example: bcppack mygame/saved mybcp.bcp 2

Tip:
In WKBattles you can make "patch BCPs". These are BCPs which only contain
the files that are modified. If you do so you can simply copy your BCP with
a cool name in the game directory without removing data.bcp. When the
game is started, it not only looks for data.bcp but also for other BCPs present
in the directory, and hopefully it will use the data from the patch BCP instead
of data.bcp if a file exists in both BCPs. This is how "patch_1_1.bcp" or
"sounds_german.bcp" work. I don't know if this trick works in the first
Warrior Kings too.

In case you are interested, the source code is in the src directory.

Have fun!
AdrienTD :)