bigcpp is a program that can pack a WK game set file (.cpp) and all files it includes
into a single game set file. That means that every definition of object types
and all scripts are now in a single game set file.

So you can find a piece of code much easier. Because everything is in the same file,
CTRL+F is the only thing you need. Previously you had to seek through
every single file, which makes you lose time.

The original game set can be replaced with the expanded one in a savegame/level
without any problems.

As such, modding and learning the game set can now be done faster.

The "cpp" files in data.bcp are in fact "game set files". Some of them also
have the ".gsf" extension instead of ".cpp". The developers used the cpp
extension instead probably because they used Visual Studio as an IDE for
their game set and if they use the cpp extension the IDE will enable certain
specific features like colors in the code.

I'll show you two methods how to create an "expanded" cpp:

Batch:
 * Extract the whole data.bcp (with the BCP viewer) if you haven't done that yet.
 * Make a backup/copy of batch.bat.
 * Be sure batch.bat is in the same directory as bigcpp.exe.
 - Launch a text editor and open batch.bat in it.
 - Modify the WKGAMESET and WKCPPFILE values.
 - Double-click batch.bat.
 - The result is written in output.txt in bigcpp's directory.

Manual:
 * Extract the whole data.bcp (with the BCP viewer) if you haven't done that yet.
 - Open a command prompt.
 - cd to the extracted "Warrior Kings Game Set" directory.
 - Type:
     [path to bigcpp.exe] [input cpp] > [output cpp]
   for example:
     C:\test\bigcpp.exe "multi-player extensions.cpp" > C:\test\mpexts.txt