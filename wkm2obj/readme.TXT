
mesh3 to obj+mtl converter by AdrienTD
Usage: wkm2obj <input file.mesh3> <output name> [-u <number>] [-p <number>]
-u: Use particular UV list (model color)
-p: Use particular polygon list
Examples: - wkm2obj input.mesh3 output
            converts input.mesh3 to files output.obj and output.mtl.
          - wkm2obj abc.mesh3 def -u 1
            converts abc.mesh3 to files def.obj and def.mtl with blue color.

Additions:

- If you want to get textures on your OBJ, you must copy the texture files to
  the OBJ file's directory. Here are the steps you need to do:
   1. Look at the MTL file to get the names of the textures needed.
   2. Find the textures in "Warrior Kings Game Set\Textures" in data.bcp.
   3. Extract and copy them to the directory with the OBJ and the MTL.

- UV list index	     Color
  --------------------------------------
  0                  Brown
  1                  Blue
  2                  Yellow
  3                  Red
  4                  Green
  5                  Purple
  6                  Orange
  7                  Aqua