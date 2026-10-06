# Paint

A simple drawing app inspired by MS Paint, built with C++ using GLFW, GLAD, Dear ImGui, and Lua (all statically linked)

## About

I created this app to make a lightweight drawing tool for quick simple drawings. Im still actively developing, so more features are to come!

## Features (so far)
- Simple brushes for drawing (circle and square, switch with `R`)
- Fill tool (toggle with `B`)
- Basic ImGui color selection with preset colors
- Brush size slider
- Undo/Redo with `CTRL` `Z` and `Y`
- New canvas with `CTRL` `N`
- Clear canvas with `CTRL` `C`
- Zooming with `CTRL` + scroll, 
- Panning on scroll or right click drag
- Custom lua config file (window/canvas size, zoom and custom colors)
- More features coming...

## Installation & Setup
### Prerequisites
- C++ compiler (C++20 or later)
- [Premake5](https://premake.github.io/)

for arch do
```bash
sudo pacman -S premake
```

### Build & Run
1. **Generate project files:**
   ```bash
   premake5 gmake
   ```
   or if you are on windows build for visual studio with
   ```bash
   premake5 vs2022
   ```
   
 3. **finally compile and run:**
    ```bash
    make -j -C Build/ config=release_x64 && ./bin/Release/Paint
    ```
