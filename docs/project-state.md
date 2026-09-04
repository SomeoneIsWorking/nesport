# Project state

## Comparison baseline

The baseline is an unmodified game running in the standalone MesenCE emulator. The intended
difference is an embeddable runtime with explicit native-handler boundaries for title ports; the
current library proof is not yet a replacement player experience.

| ID | Capability | State | Dependencies | Goals |
|---|---|---|---|---|
| S001 | Exact maintained MesenCE execution dependency | verified | — | G001 |
| S002 | Scoped native/original instruction dispatch | verified | S001 | G001 |
| S003 | Complete boundary and machine-state access | verified | S001 | G001 |
| S004 | Representative commercial-game faithfulness | missing | S001, S002, S003 | G002 |
| S005 | Desktop player delivery | missing | S004 | G003 |
| S006 | Android arm64-v8a delivery | missing | S004 | G003 |
| S007 | Host portability evidence | partial | S001, S002, S003 | G003 |

## Current focus

S004 — representative commercial-game faithfulness.

## Capability details

### S001 — Exact maintained MesenCE execution dependency

Verified means configuration refuses any revision except the lock in
`cmake/MesenCERevision.cmake`, and a synthetic iNES image executes in that core without InteropDLL.
Evidence: `tests/runtime_test.cpp` and the passing Linux x86_64 Clang/Ninja CTest.

### S002 — Scoped native/original instruction dispatch

Verified means an enabled scope can replace one guest instruction, explicitly request the ordinary
core instruction, or stop before execution, with bounded counters for both executed paths. Evidence:
`tests/runtime_test.cpp` reaches and checks all three transitions.

### S003 — Complete boundary and machine-state access

Verified means native handlers can inspect and restore the CPU's complete instruction-boundary
state, including pending IRQ/NMI and DMA fields, use direct data/write/execute callbacks, resolve a
logical address to physical media, and save/restore MesenCE's serialized full-machine state.
Evidence: the pinned MesenCE `mesence_nes_port_test` and nesport runtime test.

### S004 — Representative commercial-game faithfulness

Missing capability: no user-supplied commercial ROM has completed representative gameplay, deterministic
oracle comparison, mapper/PPU/APU coverage, or native-handler coverage accounting.

### S005 — Desktop player delivery

Missing capability: there is no standalone player, first-run ROM chooser, renderer/input/audio composition,
save UI, or packaged Windows/Linux/macOS release.

### S006 — Android arm64-v8a delivery

Missing capability: no shared Android package target, arm64-v8a APK, emulator/device runtime execution, touch
controls, or performance matrix has been produced.

### S007 — Host portability evidence

Gap: Linux x86_64 runs the synthetic core/native/state proof, but Windows, macOS x86_64/arm64,
Linux arm64, and Android arm64-v8a have no matching-host runtime evidence yet. MesenCE also retains a
process-global home-directory owner; `Machine` receives that value through typed configuration, but
multiple concurrent machines with different data roots are not yet supported.
