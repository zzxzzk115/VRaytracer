@echo off

cmake -S . -B build_windows
cmake --build build_windows --config=Release -j 8

pause