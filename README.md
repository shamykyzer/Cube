# 3D Rotating Cube

A simple C program that displays a rotating 3D cube in the terminal using ASCII characters. The program demonstrates basic 3D graphics principles, including rotation and depth buffering.

![cube2](https://github.com/user-attachments/assets/71a3aa88-8b7b-4baf-a9d9-60074fec6a60)

## Features

- Real-time 3D cube rotation
- ASCII art rendering
- Depth buffering for correct rendering of surfaces
- Windows-friendly terminal rendering (ANSI cursor control enabled on Windows)
- Auto-centers output in larger terminals and clamps to smaller terminals

## Requirements

- A C compiler/toolchain

### Windows (recommended)

- **MSYS2 MinGW-w64 GCC** (recommended, simplest to get standard headers/libs)
- PowerShell (for `build.ps1`)

### Windows (alternatives)

- **Visual Studio Build Tools** (MSVC `cl.exe` + Windows SDK)
- **LLVM clang**: works **only if** you also have MSVC/Windows SDK headers installed (otherwise you may see `fatal error: 'stdio.h' file not found`).

## How to Build and Run

### Windows (PowerShell) using `build.ps1`

From the project folder:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
.\cube.exe
```

Build + run in one command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -Run
```

If you get `Permission denied` while building, `cube.exe` is still running. Stop it (Ctrl+C in the cube window) or run:

```powershell
Stop-Process -Name cube -Force -ErrorAction SilentlyContinue
```

#### Choosing a compiler

`build.ps1` can auto-pick a compiler, or you can force one:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -Compiler gcc
```

Valid options: `auto`, `gcc`, `clang`, `cl`.

### Installing GCC via MSYS2 (Windows)

If you don't already have a compiler, MSYS2 GCC is the fastest setup.

1. Install MSYS2:

```powershell
winget install MSYS2.MSYS2
```

2. Open **“MSYS2 UCRT64”** from the Start menu, then run:

```sh
pacman -Syu
# close/reopen UCRT64 if prompted, then:
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
```

`build.ps1` knows the common MSYS2 install paths (`C:\msys64\ucrt64\bin`) so you typically **don't** need to edit PATH manually.

## Code Structure

- `cube.c`: The main source code file containing the 3D cube rotation logic.
- `build.ps1`: Windows build helper (auto-detects compilers and builds `cube.exe`).

## Explanation of Key Components

- **Rotation Angles**: Variables `A`, `B`, and `C` represent the rotation angles for the cube.
- **Cube and Screen Parameters**: The program renders into a fixed buffer (up to 160x44) and will clamp to smaller terminals.
- **Depth Buffer**: `zBuffer` tracks depth per character cell.
- **Functions**:
  - `calculateX`, `calculateY`, `calculateZ`: Calculate the X, Y, and Z coordinates after rotation.
  - `calculateForSurface`: Projects points and updates the screen buffer with the calculated coordinates and characters.

## Customization

- **Cube size**: `cubeWidth` (default is ~20% smaller than the original `20`).
- **Spin speed**: `rotSpeedA`, `rotSpeedB`, `rotSpeedC` (increase = faster).
- **Frame rate**: `frameDelayMs` (decrease = faster FPS).
- **Detail level**: `incrementSpeed` (decrease = more points, smoother but slower).

## Screenshot

![image](https://github.com/user-attachments/assets/b4751dad-1b22-407b-8f5d-4f2d3617b0eb)

![image](https://github.com/user-attachments/assets/c7f5188e-4a62-4d4f-a5c0-457678cfb30e)

## Terminal notes (Windows)

- Use a **monospace font**.
- If your terminal is narrower than the render width, the program clamps to your window size to avoid wrapping.

## Acknowledgements

Inspired by various ASCII art and 3D graphics tutorials.

For more information on rotation matrices, visit the [Wikipedia page](https://en.wikipedia.org/wiki/Rotation_matrix).

Special thanks to the [YouTube video](https://www.youtube.com/watch?v=p09i_hoFdd0) for inspiration and guidance.
