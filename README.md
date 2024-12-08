# MultiLudo - A Multithreaded Ludo Game

A modern implementation of the classic Ludo board game using C++, Raylib, and multithreading. This project demonstrates the use of threads, mutexes, and semaphores in a graphical game environment.


## System Specifications

- **Device**: Dell Inspiron 15-5510
- **OS**: Ubuntu 22.04.5 LTS (64-bit)
- **Processor**: 11th Gen Intel® Core™ i7-11390H @ 3.40GHz × 8
- **Memory**: 8.0 GiB
- **Graphics**: Mesa Intel® Xe Graphics (TGL GT2)
- **Disk Capacity**: 512.1 GB
- **GNOME Version**: 42.9
- **Windowing System**: X11

## Features

- 🎮 Modern graphical interface using Raylib
- 🧵 Multithreaded gameplay for smooth performance
- 👥 Support for 1-4 players
- 🎲 Dice rolling system
- 🎯 Token movement with collision detection
- 🏆 Winner tracking and scoring system
- 🎨 Color-coded player tokens (Red, Green, Yellow, Blue)

## Project Structure

```
MultiLudo/
├── assets/               # Game assets (images)
│   ├── board1.png       # Game board image
│   ├── 1-dice.png       # Dice face images
│   ├── 2-dice.png       # Dice face images
│   ├── 3-dice.png       # Dice face images
│   ├── 4-dice.png       # Dice face images
│   ├── 5-dice.png       # Dice face images
│   ├── 6-dice.png       # Dice face images
│   ├── blue-goti.png    # Player tokens
│   ├── green-goti.png   # Player tokens
│   ├── yellow-goti.png  # Player tokens
│   └── red-goti.png     # Player tokens
├── include/             # Header files
│   ├── Game.h          # Game class declaration
│   ├── Player.h        # Player class declaration
│   ├── Token.h         # Token class declaration
│   └── Utils.h         # Utility functions and globals
├── src/                # Source files
│   ├── Game.cpp        # Game class implementation
│   ├── Player.cpp      # Player class implementation
│   ├── Token.cpp       # Token class implementation
│   ├── Utils.cpp       # Utility functions implementation
│   └── main.cpp        # Main entry point
├── CMakeLists.txt      # CMake build configuration
├── build.sh            # Build script
├── .gitignore         # Git ignore file
└── README.md          # This file
```

## Prerequisites

### Required Packages
1. C++ Compiler and Build Tools:
```bash
sudo apt update
sudo apt install build-essential cmake
```

2. Raylib Dependencies:
```bash
sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
```

3. Git (for cloning):
```bash
sudo apt install git
```

### Installing Raylib

#### Option 1: Using Package Manager (Recommended)
```bash
sudo apt install libraylib-dev
```

#### Option 2: Building from Source
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

## Building the Game

1. Clone the repository:
```bash
git clone <your-repository-url>
cd MultiLudo
```

2. Run the build script:
```bash
chmod +x build.sh
./build.sh
```

3. Run the game:
```bash
cd build
./MultiLudo
```

## How to Play

1. **Starting the Game**
   - Launch the game
   - Select number of players (1-4)
   - Click "START" to begin

2. **Game Rules**
   - Players take turns rolling the dice
   - Roll a 6 to move a token out of home
   - Move tokens clockwise around the board
   - Land on other players' tokens to send them back
   - Get all tokens to home base to win

3. **Controls**
   - Left Click: Roll dice / Select token

## Code Documentation

### Key Components

1. **Game Class (`Game.h`, `Game.cpp`)**
   - Manages game state and rendering
   - Handles window and asset management
   - Controls game flow and screens

2. **Player Class (`Player.h`, `Player.cpp`)**
   - Manages player state and tokens
   - Handles player turns and moves
   - Controls token collisions

3. **Token Class (`Token.h`, `Token.cpp`)**
   - Manages individual token behavior
   - Handles token movement and position
   - Controls token state (home/out/finished)

4. **Utils (`Utils.h`, `Utils.cpp`)**
   - Global variables and utility functions
   - Turn management
   - Grid position calculations

### Threading Model

- Main Thread: Window and rendering
- Player Threads: Individual player actions
- Mutex Protection: Dice rolling and turn management
- Semaphores: Token movement synchronization

## Troubleshooting

1. **Build Issues**
   - Ensure all dependencies are installed
   - Check CMake version (`cmake --version`)
   - Verify Raylib installation (`pkg-config --modversion raylib`)

2. **Runtime Issues**
   - Verify assets are in correct location
   - Check file permissions
   - Monitor system resources

3. **Common Solutions**
   ```bash
   # Rebuild from clean state
   rm -rf build
   ./build.sh
   
   # Check asset permissions
   ls -l build/assets
   
   # Verify library links
   ldd build/MultiLudo
   ```

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Raylib for the graphics library
- Original Ludo game concept
