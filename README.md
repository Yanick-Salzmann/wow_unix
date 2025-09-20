# WoW Unix

A modern cross-platform World of Warcraft client implementation for Unix-like systems, built with C++ and Angular.

## Overview

WoW Unix is a comprehensive reimplementation of the World of Warcraft client specifically designed for Unix-like operating systems. The project combines a high-performance C++ backend with a modern Angular-based user interface, leveraging the Chromium Embedded Framework (CEF) for seamless web-native UI integration.

## Architecture

### Core Components

#### Backend (C++)
- **OpenGL Rendering Engine**: Custom OpenGL-based rendering system with shader support
- **CEF Integration**: Chromium Embedded Framework for web UI rendering
- **MPQ Archive Support**: Full MPQ file format support using StormLib for reading game assets
- **DBC Database System**: Complete database client (DBC) file parsing and management
- **Event-Driven Architecture**: Protobuf-based communication between frontend and backend
- **Dependency Injection**: Boost.DI-based modular architecture

#### Frontend (Angular)
- **Modern Angular 20**: Standalone components with TypeScript
- **Protobuf Communication**: Type-safe communication with C++ backend
- **Responsive UI**: SCSS-based styling with component modularity
- **CEF Bridge**: Seamless integration with native backend through CEF message routing

### Key Technologies

- **C++23**: Modern C++ standards with extensive STL usage
- **OpenGL 4.x**: Hardware-accelerated 3D rendering
- **GLFW**: Cross-platform window and input management
- **CEF (Chromium Embedded Framework)**: Web technology integration
- **StormLib**: MPQ archive file format support
- **Protocol Buffers**: Structured data serialization
- **Angular 20**: Modern web framework with TypeScript
- **Boost Libraries**: DI container and utility libraries
- **spdlog**: High-performance logging framework

## Project Structure

```
wow-unix/
├── src/                          # C++ source code
│   ├── main.cpp                  # Primary application entry point
│   ├── main_browser.cpp          # CEF browser process entry point
│   ├── gl/                       # OpenGL rendering system
│   │   ├── window.h/cpp          # GLFW window management
│   │   ├── mesh.h/cpp            # 3D mesh handling
│   │   ├── texture.h/cpp         # Texture management
│   │   ├── program.h/cpp         # Shader program management
│   │   └── shared_texture.h/cpp  # CEF-OpenGL texture sharing
│   ├── web/                      # CEF integration layer
│   │   ├── web_core.h/cpp        # Core web rendering system
│   │   ├── web_client.h/cpp      # CEF client implementation
│   │   ├── web_application.h/cpp # CEF application handler
│   │   ├── app_scheme_handler.h/cpp # Custom URL scheme handler
│   │   └── event/                # Event system
│   ├── io/                       # File I/O systems
│   │   ├── mpq_manager.h/cpp     # MPQ archive management
│   │   ├── mpq_file.h/cpp        # MPQ file abstraction
│   │   └── dbc/                  # Database client file support
│   ├── utils/                    # Utility systems
│   │   ├── di.h/cpp              # Dependency injection setup
│   │   └── string_utils.h/cpp    # String manipulation utilities
│   └── proto/                    # Protocol buffer definitions
│       └── js_event.proto        # Frontend-backend communication schema
├── ui/                           # Angular frontend
│   ├── src/app/                  # Angular application
│   │   ├── app.ts                # Root application component
│   │   ├── splash/               # Splash screen component
│   │   ├── loading/              # Loading screen component
│   │   ├── service/              # Angular services
│   │   │   └── event.service.ts  # CEF communication service
│   │   └── proto/                # Generated TypeScript protobuf types
│   ├── package.json              # Node.js dependencies
│   └── angular.json              # Angular build configuration
├── cef/                          # CEF framework files
├── shaders/                      # OpenGL shaders
│   ├── ui_vertex.glsl           # UI vertex shader
│   └── ui_fragment.glsl         # UI fragment shader
└── CMakeLists.txt               # Build system configuration
```

## Features

### Rendering System
- **Hardware-accelerated OpenGL rendering** with modern shader pipeline
- **CEF-OpenGL texture sharing** for seamless web UI integration
- **Custom mesh and texture management** optimized for game assets
- **Cross-platform window management** through GLFW

### Asset Management
- **Complete MPQ archive support** for reading original WoW game files
- **DBC database parsing** for game data structures
- **Efficient file caching** and memory management
- **Locale-aware asset loading**

### User Interface
- **Modern Angular-based UI** with TypeScript and SCSS
- **Component-driven architecture** with standalone Angular components
- **Real-time communication** between UI and game engine
- **Responsive design** optimized for various screen sizes

### Communication Layer
- **Protocol Buffers** for type-safe frontend-backend communication
- **Event-driven messaging** system with persistent connections
- **Custom CEF scheme handler** for app:// protocol
- **Bidirectional message routing** between processes

## Building

### Prerequisites
- **CMake 3.24+**
- **C++23 compatible compiler** (GCC 12+, Clang 15+)
- **OpenGL 4.x compatible graphics drivers**
- **GTK3 development libraries**
- **Protocol Buffers compiler**
- **Node.js 18+** and npm for UI build

### Backend Build
```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make -j$(nproc)
```

### Frontend Build
```bash
cd ui
npm install
npm run build
```

### Dependencies
The project automatically fetches and builds the following dependencies:
- **spdlog** (v1.15.3) - Logging framework
- **GLFW** (latest) - Window and input management
- **GLAD** (v0.1.36) - OpenGL loader
- **GLM** (latest) - Mathematics library
- **StormLib** (latest) - MPQ archive support
- **STB** (latest) - Image loading utilities

## Configuration

### Game Client Setup
1. Launch the application
2. Specify the path to your World of Warcraft installation directory
3. The application will automatically detect and load game assets from MPQ archives

### Supported WoW Versions
The application is designed to work with various World of Warcraft client versions through configurable MPQ loading and DBC parsing.

## Development

### Code Style
- **No comments policy**: Code should be self-documenting
- **Modern C++ practices**: Extensive use of STL, smart pointers, and C++23 features
- **Minimal diffs**: Only modify necessary code sections
- **Component architecture**: Both C++ and Angular follow modular design patterns

### Event System
The application uses a sophisticated event system based on Protocol Buffers:
- **InitializeRequest/Response**: Application startup
- **BrowseFolderRequest/Response**: File system browsing
- **LoadDataEvent**: Asset loading notifications
- **LoadUpdateEvent**: Progress updates with percentage and status

### Adding New Features
1. Define new message types in `js_event.proto`
2. Regenerate Protocol Buffer bindings for both C++ and TypeScript
3. Implement handlers in both backend (`web_core.cpp`) and frontend (`event.service.ts`)
4. Add UI components as needed in the Angular application

## Architecture Benefits

- **Performance**: Native C++ engine with hardware-accelerated OpenGL rendering
- **Modern UI**: Web technologies provide flexible, maintainable user interfaces
- **Cross-platform**: GLFW and CEF enable broad Unix compatibility
- **Maintainable**: Clear separation between rendering, asset management, and UI layers
- **Extensible**: Event-driven architecture allows easy feature additions
- **Type-safe**: Protocol Buffers ensure reliable communication between components

## License

This project is designed for educational and development purposes related to understanding World of Warcraft client architecture and cross-platform game development techniques.

## Contributing

When contributing to this project:
1. Follow the established code style (no comments, minimal diffs)
2. Ensure all changes maintain cross-platform compatibility
3. Test both the C++ backend and Angular frontend
4. Update Protocol Buffer definitions when adding new communication features
5. Maintain the modular architecture principles

---

*Built with modern C++23, Angular 20, and a passion for cross-platform game development.*
