
echo ******** build start *********

SETLOCAL
SET MY_PATH=%~dp0

cd ..
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A x64 

cmake --build .

pause