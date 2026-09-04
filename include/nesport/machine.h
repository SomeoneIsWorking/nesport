#pragma once

#include "nesport/config.h"
#include "nesport/logger.h"
#include "nesport/native_overrides.h"

#include "Shared/Emulator.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class NesConsole;

namespace nesport {
struct RunResult final {
	NesPortRunStop Stop = NesPortRunStop::BudgetExhausted;
	uint64_t CoreInstructions = 0;
	uint64_t NativeInstructions = 0;
	uint16_t ProgramCounter = 0;
};

class Machine final {
  public:
	Machine(RuntimeConfig config, ILogger &logger, NativeOverrideRegistry &overrides);
	~Machine();

	Machine(const Machine &) = delete;
	Machine &operator=(const Machine &) = delete;
	Machine(Machine &&) = delete;
	Machine &operator=(Machine &&) = delete;

	void LoadRom(const std::vector<uint8_t> &rom, const std::string &displayName);
	RunResult Run();
	std::vector<uint8_t> SaveState() const;
	void LoadState(const std::vector<uint8_t> &state);
	uint8_t ReadMemory(uint16_t address) const;
	void WriteMemory(uint16_t address, uint8_t value);

  private:
	class ExecutionHook;

	RuntimeConfig _config;
	ILogger &_logger;
	NativeOverrideRegistry &_overrides;
	std::unique_ptr<Emulator> _emulator;
	NesConsole *_console = nullptr;

	static RuntimeConfig ValidateConfig(RuntimeConfig config);
	static std::unique_ptr<Emulator> CreateEmulator(const RuntimeConfig &config);
	void RequireLoaded() const;
};
} // namespace nesport
