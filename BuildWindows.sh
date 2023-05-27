#!/bin/bash

cmake -S . -B build_windows
cmake --build build_windows --config=Release -j 8