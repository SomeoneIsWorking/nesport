# nesport structure

- `Machine` composes the maintained core, typed configuration, logger, and native registry; it does
  not absorb title behavior or frontend subsystems.
- Guest CPU, interrupt, DMA, mapper, and full-machine serialization semantics stay in the maintained
  MesenCE fork. Do not reproduce them in this repository.
- Product code receives `RuntimeConfig` and `ILogger`; it must not read the environment or write
  directly to a process stream.
- Native handlers are registered by guest address and explicit scope. Preserve typed
  original/native/stop transitions and bounded execution accounting.
- Do not add offline guest-source generators, derived guest-code corpora, backend selection flags,
  or an alternate CPU implementation. The maintained MesenCE execution path is the sole ordinary
  path.
- Automation is modular Python. `run.sh` is allowed only when a real player target exists.
