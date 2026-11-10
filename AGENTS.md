# nesport structure

- `Machine` composes the maintained core, typed configuration, logger, and native registry; it does
  not absorb title behavior or frontend subsystems.
- Guest CPU, interrupt, DMA, mapper, and full-machine serialization semantics stay in the maintained
  MesenCE fork. Do not reproduce them in this repository. Its execution path is the sole ordinary
  path: no backend selection flags or alternate CPU implementation.
- Product code receives `RuntimeConfig` and `ILogger`.
- Native handlers are registered by guest address and explicit scope. Preserve typed
  original/native/stop transitions and bounded execution accounting.
- No `run.sh` until a real player target exists.
- Full verifier: `uv run --frozen python tools/verify.py`.
