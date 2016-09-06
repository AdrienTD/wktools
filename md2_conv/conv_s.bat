mkdir %1
cd %1
..\md2_to_wkm.exe ..\%1.md2 %1.mesh3 0 %1.tga 23 %2
..\md2_to_wka.exe ..\%1.md2 default.anim3 %1.mesh3 0 40 23 %2
..\md2_to_wka.exe ..\%1.md2 move.anim3 %1.mesh3 40 6 23 %2
..\md2_to_wka.exe ..\%1.md2 attack_straight.anim3 %1.mesh3 46 8 23 %2
..\md2_to_wka.exe ..\%1.md2 take_hit1.anim3 %1.mesh3 54 4 23 %2
..\md2_to_wka.exe ..\%1.md2 take_hit2.anim3 %1.mesh3 58 4 23 %2
..\md2_to_wka.exe ..\%1.md2 take_hit3.anim3 %1.mesh3 62 4 23 %2
..\md2_to_wka.exe ..\%1.md2 idle1.anim3 %1.mesh3 84 11 23 %2
..\md2_to_wka.exe ..\%1.md2 idle2.anim3 %1.mesh3 95 17 23 %2
..\md2_to_wka.exe ..\%1.md2 idle3.anim3 %1.mesh3 112 11 23 %2
..\md2_to_wka.exe ..\%1.md2 shoot_straight.anim3 %1.mesh3 123 12 23 %2

mkdir Dead
cd Dead
mkdir Death1
cd Death1
..\..\..\md2_to_wkm.exe ..\..\..\%1.md2 default.mesh3 183 %1.tga 23 %2
..\..\..\md2_to_wka.exe ..\..\..\%1.md2 death.anim3 default.mesh3 178 6 23 %2 0
cd..
mkdir Death2
cd Death2
..\..\..\md2_to_wkm.exe ..\..\..\%1.md2 default.mesh3 189 %1.tga 23 %2
..\..\..\md2_to_wka.exe ..\..\..\%1.md2 death.anim3 default.mesh3 184 6 23 %2 0
cd..
mkdir Death3
cd Death3
..\..\..\md2_to_wkm.exe ..\..\..\%1.md2 default.mesh3 197 %1.tga 23 %2
..\..\..\md2_to_wka.exe ..\..\..\%1.md2 death.anim3 default.mesh3 190 8 23 %2 0
cd..
cd..

cd..