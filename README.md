# nesport

`nesport` is the shared NES runtime for native game-port projects. It embeds a maintained MesenCE
core directly, runs the original ROM through that core, and permits narrowly scoped native behavior
at instruction boundaries. It does not use MesenCE's process-global InteropDLL API.

The current milestone is a library and synthetic runtime proof, not a player-ready emulator or game
port. On Linux x86_64, the proof loads an asset-free iNES image through the shipping MesenCE core,
runs ordinary 6502 instructions, replaces one instruction with a scoped native handler, stops at a
bounded instruction boundary, and restores a full machine snapshot. Representative commercial-game
play, desktop packaging, Apple Silicon, Windows, and Android remain unverified.

## Dependency

The maintained MesenCE fork is `https://github.com/SomeoneIsWorking/MesenCE.git`. `nesport` requires
the exact revision recorded in `cmake/MesenCERevision.cmake` and refuses a different or locally
modified checkout, including untracked files. By default it resolves the sibling directory `../mesence`; callers can
set the CMake cache path `MESENCE_SOURCE_DIR` explicitly.

## Verification

Required native packages on Fedora are provided by `clang`, `cmake`, `ninja-build`, and
`SDL2-devel`. Run:

```text
uv run --frozen python tools/verify.py
```

The project deliberately has no `run.sh`: there is not yet a genuine standalone player target to
launch. Tests synthesize their own freely generated ROM bytes and no game data is stored here.

## License

`nesport` is licensed under GPL-3.0-or-later because it links MesenCE. See `LICENSE` and the pinned
MesenCE checkout for notices and corresponding source.
