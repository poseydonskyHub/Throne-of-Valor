# Trone of Valor

Trone of Valor is a small console game written in C++.
(now project windows-specific)
(This is my first project, so don't judge me too harshly.)

## Features

- Multiple playable classes
- Level progression through portals
- Enemy projectiles
- Healing pickups
- Boss fight
- Colored console rendering

## Project Structure

- `source/main.cpp` - main file
- `headers/classes.hpp` - classes
- `headers/unit.hpp` - main unit class
- `levels/*.txt` - level files

## Build With CMake

```bash
cmake -S . -B build
cmake --build build
