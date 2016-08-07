With BCPView you can open BCP pack files and extract files from them.

It can extract files of the following formats:
 1: Uncompressed
 2: Empty file
 4: LZRW3-compressed files

bzip2-compressed files (format 3) are not directly decompressed at the moment.
After extracting a bzip2-compressed file with bcpview, you can use the
commandline bzip2 tool with the -d flag to decompress the file.

Quick start
---------------

 - Run bcpview.exe.
 - Click File > Open and select a BCP file.

Now you can see the whole structure of the BCP in a tree view. Click the +
button to expand the directory or click the - button to collapse it.

Double-click a file to extract it. It will be saved to the "bcpxtract"
directory.

Right-click a file to copy its path in the clipboard.

Click "Tools > Extract all files..." to extract all the files to the
"bcpxtract" directory.