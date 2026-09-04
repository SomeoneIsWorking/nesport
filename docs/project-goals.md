# Project goals

## G001 — Reusable native-hybrid NES runtime

Provide title ports with a maintained full-system NES core and scoped native overrides without
offline-generated guest source.

Success conditions:

- original ROM instructions execute through the maintained MesenCE core;
- native handlers enter at exact instruction boundaries and can run the original instruction;
- CPU, interrupt, DMA, memory-mapping, and full-machine state remain coherent across transitions;
- no process-global InteropDLL instance or runtime backend selector is part of the port API.

Constraints and non-goals: this layer owns reusable NES execution, not title logic, copyrighted ROM
bytes, a second NES emulator, or a generated-source translation pipeline.

Related state: S001, S002, S003.

## G002 — Faithful and measurable game execution

Qualify real games through representative gameplay rather than boot screens or video playback.

Success conditions:

- deterministic oracle comparisons cover CPU, mapper, PPU, APU, input, and save behavior;
- representative gameplay reports separate ordinary-core and native-handler coverage;
- native changes remain individually disableable for original-path comparisons.

Related state: S004.

## G003 — Portable host delivery

Support x86_64 and arm64 on Linux, Windows, macOS, and Android without bundling game data.

Success conditions:

- matching-host tests execute the synthetic runtime proof on every supported platform;
- Android arm64-v8a builds and runs through shared Android packaging/runtime owners;
- player packages provide no-terminal ROM selection, validation, persistent configuration, input,
  audio, video, and saves.

Related state: S005, S006, S007.
