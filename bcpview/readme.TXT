+---------------------+
| BCPView by AdrienTD |
+---------------------+

With BCPView you can open BCP pack files and extract files from them.

It can extract files of the following formats:
 1: Uncompressed
 2: Empty file
 3: bzip2-compressed file
 4: LZRW3-compressed file


Quick start
---------------

 - Run bcpview.exe.
 - Click File > Open and select a BCP file.

Now you can see the whole structure of the BCP in a tree view. Click the +
button to expand the directory or click the - button to collapse it.

Double-click a file to extract it. It will be saved to the "bcpxtract"
directory.

Right-click a file to copy its path into the clipboard.

Click "Tools > Extract all files..." to extract all the files to the
"bcpxtract" directory.


What's new in version 1.0?
------------------------------

 - bzip2 decompression added.
 - Date and time of the selected file is shown in the status bar.
 - Image/Icon list changed (L for LZRW3, B for bzip2)
 - Other small changes...


Known issues
----------------

 - Unicode-only characters in file names will not be displayed correctly.
