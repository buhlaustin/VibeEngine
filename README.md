# Vibe Engine

Proprietary, open-source game engine built for modern workflows: install the tools, create a project on disk (optionally on GitHub), and build games for PC, mobile, and console from the same project.

This repository is the **engine platform** — runtime, project format, CLI, and (soon) desktop editor.

## Requirements

- CMake 3.20+
- C++20 compiler (GCC or Clang)
- [SDL2](https://www.libsdl.org/) development libraries
- Rust (for the `vibe` CLI)

On Ubuntu:

```bash
sudo apt-get install -y libsdl2-dev
```

## Quick start

Build the runtime and CLI from the repo root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
cmake --build build
cargo build --release --manifest-path tools/vibe/Cargo.toml
export PATH="$PWD/target/release:$PATH"
```

Create a new game project and run it:

```bash
vibe init ../MyGame --name MyGame
vibe run ../MyGame
```

Close the window or press the window manager close control to exit.

## Project layout

| Path | Purpose |
|------|---------|
| `engine/` | C++ runtime (platform, project loader, scenes) |
| `tools/vibe/` | `vibe` CLI — `init`, `build-engine`, `run`, `doctor` |
| `templates/` | New project templates |
| `editor/` | Desktop editor (stub — in progress) |

## Project format

Games are folders with a **`vibe.project.json`** manifest and text **`*.scene.json`** files (git-friendly, agent-friendly). Engine caches and local build output live under **`.vibe/`** and are not meant for version control.

## CLI

```bash
vibe init <path> [--name MyGame]
vibe build-engine [--profile Debug|Release]
vibe run [project-path]
vibe doctor [project-path]
```

## License

MIT — see [LICENSE](LICENSE).
