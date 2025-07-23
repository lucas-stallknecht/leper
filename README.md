# Leper

Leper is an OpenGL-based action-adventure game inspired by Pokémon Ranger, combining taming with skill-based combat and parry mechanics.

## Build instructions: 

Dependencies : CMake 3.21 or higher and Ninja
1. Clone the repository (with submodules):
```bash
git clone --recurse-submodules https://github.com/lucas-stallknecht/leper.git
cd leper
```

2. Create a build directory and compile the project :
```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

3. Run the executable:
```bash
./leper
```
