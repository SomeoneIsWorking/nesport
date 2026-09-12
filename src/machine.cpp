#include "nesport/machine.h"

#include "NES/NesConsole.h"
#include "NES/Port/NesPortExecution.h"
#include "Utilities/FolderUtilities.h"
#include "Utilities/VirtualFile.h"

#include <stdexcept>
#include <utility>

namespace nesport {
class Machine::ExecutionHook final : public INesPortInstructionHook {
  public:
	ExecutionHook(const RuntimeConfig &config, ILogger &logger, const NativeOverrideRegistry &overrides)
		: _config(config), _logger(logger), _overrides(overrides) {
	}

	NesPortInstructionAction OnBeforeInstruction(NesPortInstructionContext &context) override {
		uint16_t programCounter = context.GetCpuState().Cpu.PC;
		const NativeOverride *nativeOverride = _overrides.Find(programCounter, _config.ActiveOverrideScopes);
		if (!nativeOverride) {
			return NesPortInstructionAction::Continue;
		}

		_logger.Write(LogLevel::Debug, nativeOverride->Name);
		NativeCall call(context);
		switch (nativeOverride->Handler(call)) {
		case NativeOverrideAction::Handled:
			return NesPortInstructionAction::NativeHandled;
		case NativeOverrideAction::RunOriginal:
			return NesPortInstructionAction::Continue;
		case NativeOverrideAction::Stop:
			return NesPortInstructionAction::Stop;
		}
		throw std::logic_error("native override returned an invalid action");
	}

  private:
	const RuntimeConfig &_config;
	ILogger &_logger;
	const NativeOverrideRegistry &_overrides;
};

Machine::Machine(RuntimeConfig config, ILogger &logger, NativeOverrideRegistry &overrides)
	: _config(ValidateConfig(std::move(config))), _logger(logger), _overrides(overrides), _emulator(CreateEmulator(_config)) {
}

Machine::~Machine() {
	_emulator->Release();
}

void Machine::LoadRom(const std::vector<uint8_t> &rom, const std::string &displayName) {
	if (rom.empty()) {
		throw std::invalid_argument("ROM bytes must not be empty");
	}
	if (displayName.empty()) {
		throw std::invalid_argument("ROM display name must not be empty");
	}
	if (!_emulator->LoadRom(VirtualFile(rom.data(), rom.size(), displayName), VirtualFile(), false)) {
		throw std::runtime_error("MesenCE rejected the supplied NES ROM");
	}
	_console = dynamic_cast<NesConsole *>(_emulator->GetConsoleUnsafe());
	if (!_console) {
		throw std::runtime_error("MesenCE did not load an NES console");
	}
	_logger.Write(LogLevel::Info, "NES ROM loaded");
}

RunResult Machine::Run() {
	RequireLoaded();
	ExecutionHook hook(_config, _logger, _overrides);
	NesPortRunResult core = _console->RunPortInstructions(_config.InstructionBudget, hook);
	return {core.Stop, core.CoreInstructions, core.NativeInstructions, core.ProgramCounter};
}

std::vector<uint8_t> Machine::SaveState() const {
	RequireLoaded();
	return _console->SavePortMachineState();
}

void Machine::LoadState(const std::vector<uint8_t> &state) {
	RequireLoaded();
	if (_console->LoadPortMachineState(state) != NesPortStateLoadResult::Loaded) {
		throw std::invalid_argument("MesenCE rejected the supplied machine state");
	}
}

uint8_t Machine::ReadMemory(uint16_t address) const {
	RequireLoaded();
	return _console->DebugRead(address);
}

void Machine::WriteMemory(uint16_t address, uint8_t value) {
	RequireLoaded();
	_console->DebugWrite(address, value, true);
}

void Machine::RequireLoaded() const {
	if (!_console) {
		throw std::logic_error("no NES ROM is loaded");
	}
}

RuntimeConfig Machine::ValidateConfig(RuntimeConfig config) {
	if (config.InstructionBudget == 0) {
		throw std::invalid_argument("instruction budget must be greater than zero");
	}
	if (config.UserDataDirectory.empty()) {
		throw std::invalid_argument("user data directory must not be empty");
	}
	return config;
}

std::unique_ptr<Emulator> Machine::CreateEmulator(const RuntimeConfig &config) {
	FolderUtilities::SetHomeFolder(config.UserDataDirectory);
	auto emulator = std::make_unique<Emulator>();
	emulator->Initialize(false);
	return emulator;
}
} // namespace nesport
