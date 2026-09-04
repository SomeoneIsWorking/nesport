# Codemap

Dependency direction:

```text
title port -> nesport::Machine -> maintained MesenCE NesConsole/NesCpu
                   |
                   +-> NativeOverrideRegistry -> title native handlers
                   +-> ILogger                 -> configured diagnostic sink
```

| Subsystem | Responsibility | Location | Entry point | Deep doc |
|---|---|---|---|---|
| Runtime composition | Own emulator lifetime, ROM loading, bounded execution, and state | `include/nesport/machine.h`, `src/machine.cpp` | `nesport::Machine` | `docs/project-state.md` |
| Native overrides | Address/scope registration and original/native/stop policy | `include/nesport/native_overrides.h`, `src/native_overrides.cpp` | `NativeOverrideRegistry::Register` | — |
| Typed configuration | Immutable instruction budget, user-data root, active scopes | `include/nesport/config.h` | `RuntimeConfig` | — |
| Logging boundary | Caller-provided diagnostic sink | `include/nesport/logger.h` | `ILogger::Write` | — |
| MesenCE resolver | Exact maintained dependency identity and composition | `cmake/ResolveMesenCE.cmake` | CMake include | `README.md` |
| Verification | Policy, formatting, lint, build, and runtime proof | `tools/verification/`, `tools/verify.py` | `tools/verify.py` | — |

## Placement index

- Guest CPU/IRQ/DMA boundary state and physical mapping stay in the maintained MesenCE fork.
- Title-independent native dispatch policy goes in `NativeOverrideRegistry`.
- Title-specific native behavior stays in the consuming title repository.
- Environment, CLI, and config-file ingestion belongs to a future application config owner; runtime
  modules receive only `RuntimeConfig`.
- Standalone UI, input, audio, video, and packaging owners are absent until a real player is built.
