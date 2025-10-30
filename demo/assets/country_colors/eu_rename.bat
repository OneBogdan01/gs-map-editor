@echo off
echo Renaming files to match expected lookup names...
echo.

REM Rename files to match what your lookup system expects
ren "Fezzan.txt" "Djerid.txt"
ren "Bitlis.txt" "Hisn Kayfa.txt"
ren "Janjira.txt" "Janjiro.txt"
ren "Kiche.txt" "Tzotzil.txt"
ren "Egypt.txt" "Mameluks.txt"
ren "Donauwoerth.txt" "Dortmund.txt"

echo.
echo Renamed files to match expected names!
echo.
echo NOTE: Still missing files for "Karabakh" and "Segu"
echo These files don't exist in your directory but your lookup expects them.
echo You may need to create these files or check if they have different names.
echo.
pause