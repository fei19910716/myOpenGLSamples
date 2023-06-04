
echo ******** build start *********

SETLOCAL
SET MY_PATH=%~dp0

cd %MY_PATH%/..

if exist build\ (
  echo Using exist build folder 
) else (
  mkdir build
)

cd build
cmake .. -G "Visual Studio 15 2017" -A x64 -DBUILD_ALL_SAMPLES=ON

if %ERRORLEVEL% == 1 (
    del CMakeCache.txt
    rmdir CMakeFiles
    cmake .. -G "Visual Studio 17 2022" -T host=x64 -A x64 -DBUILD_ALL_SAMPLES=ON
)
echo "-----generate sln completed------"

cmake --build .

pause