
<!-- [![RenWeb ~ a FOSS Software SDK](docs/assets/renweb.png)](https://github.com/SpurSlicer/Mauve) -->
<p align="center">
    <a href="https://github.com/SpurSlicer/Mauve">
        <img src="https://github.com/SpurSlicer/RenWeb/blob/main/docs/assets/renweb.png" alt="Renweb ~ A FOSS Software SDK">
    </a>
</p>

(*Worflow badges will go here*)

- [Introduction](#introduction)
- [Compilation](#compilation)
  - [Linux](#linux)
  - [MacOS](#macos)
  - [Windows](#windows)
- [Server-Side Design](#server-side-design)
  - [Passing Arguments](#passing-arguments)
  - [Info](#info)
  - [Logging](#logging)
  - [Web Server](#web-server)
  - [Windows](#windows-1)
- [Client Design](#client-design)
  - [Vanilla](#vanilla)
  - [Node](#node)
- [Limitations](#limitations)
- [Planned Additions](#planned-additions)
- [Credits](#credits)
- [License](#license)


## Introduction
RenWeb is a combined engine and toolset for developing GUI applications with web technology written mostly in C++.
<p align="center">
        <img src="https://github.com/SpurSlicer/RenWeb/blob/main/docs/assets/window_example_1.png" alt="Renweb ~ A FOSS Software SDK">
</p>

The RenWeb engine is built with multithreaded web server,  a process manager, all main window technologies, a logger, and more! It takes on a portable structure, searching for files in whatever directory it is placed in. In addition, RenWeb aims to provide full support for *front-end* node libraries as well as JS frameworks like react, offering for more sophisticated development.

The RenWeb engine relies on the following:
- <a href="https://github.com/webview/webview">webview</a>
- <a href="https://github.com/gabime/spdlog">spdlog</a>
- <a href="https://github.com/nlohmann/json">Nlohmann's JSON Library</a>
- <a href="https://www.boost.org/">Boost Libraries</a>
- <a href="https://en.cppreference.com/w/cpp/standard_library.html">STD Libraries</a>

For compiling the engine, RenWeb uses <a href="https://www.gnu.org/software/make/">make</a>. For building UIs, RenWeb uses <a href="https://nodejs.org/en">node</a> *only* for webpacking, TypeScript compilation, and script running via the package.jsons.

In order to use node modules for UI design (or anything like react), RenWeb needs to use a webpacker to essentially pre-process all web content into something the webview engine can read and display. RenWeb always tries to compile to standalone JS, HTML, CSS, and resource files in order to promote customizability for users of RenWeb-made applications. Customization is further discussed in it's specific section along the UI design process.

## Compilation
Every OS needs you to install the following:
- *Download and install A C++ Compiler* 
  - `g++` is recommended and is used by the makefile by default 
- *Download and install* <a href="https://nodejs.org/en/download">Node</a>
- *Download and install* <a href="https://www.gnu.org/software/make/">Make</a>

### Linux
- **Debian-based**
  - *Download and install* <a href="https://github.com/webview/webview">webview</a>
    - `apt install libgtk-3-dev libwebkit2gtk-4.1-dev`
    - `apt install libgtk-3-0 libwebkit2gtk-4.1-0`
    - `apt install pkg-config`
  - `apt install libspdlog-dev`
  - *Download and install* <a href="https://github.com/nlohmann/json">nholmann's json library</a>
  - `apt install libboost-all-dev`
  - *Make sure C Standard Libraries are installed*
- **Fedora-based**
  - *Download and install* <a href="https://github.com/webview/webview">webview</a>
    - `dnf install gtk3-devel webkit2gtk4.1-devel`
    - `dnf install gtk3 webkit2gtk4.1`
    - `dnf install pkgconf-pkg-config`
  - `dnf install spdlog`
  - *Download and install* <a href="https://github.com/nlohmann/json">nholmann's json library</a>
  - `dnf install boost-devel `
  - *Make sure C Standard Libraries are installed*
- **Other:**
  - *Download and install* <a href="https://github.com/webview/webview">webview</a>
  - *Download and install* <a href="https://github.com/gabime/spdlog?tab=readme-ov-file#package-managers">spdlog</a>
  - *Download and install* <a href="https://github.com/nlohmann/json">nholmann's json library</a>
  - *Download and install* <a href="https://www.boostlibraries.org/users/download/">All Boost Libraries</a>
  - *Make sure C Standard Libraries are installed*

### MacOS
*<p style="color: red">WIP</p>*
### Windows
*<p style="color: red">WIP</p>*

## Server-Side Design

### Passing Arguments
### Info
### Logging
### Web Server
### Windows

## Client Design
### Vanilla
### Node

## Limitations
## Planned Additions

## Credits
## License
