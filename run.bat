@echo off 
title Urban-SemSeg
echo Urban-SemSeg

set arg1=%1
echo %arg1%

set outdir=%2

set ransac_path="C:\Users\danie\Devel\src\Urban-SemSeg\build-windows\ransac\Release\"
set ransac_exe="ransac.exe"

cd %ransac_path%

for /r %arg1% %%i in (*) do (
    echo %%i

    if exist %outdir%\%%~ni\ (
        echo "Done. SKIP."
    ) else (
        mkdir %outdir%\%%~ni
        %ransac_exe% -i "%%i" -P -C -S -o %outdir%\%%~ni
    )
)