@echo OFF
cl /Fewkm3v.exe /I"%DXSDK_DIR%\Include" *.cpp user32.lib d3d9.lib winmm.lib "%DXSDK_DIR%\Lib\%TARGET_CPU%\d3dx9.lib" /O2 /MD