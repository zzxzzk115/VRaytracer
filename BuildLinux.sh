#!/bin/bash
cmake -S . -B build_linux
cmake --build build_linux --config=Release -j 8
