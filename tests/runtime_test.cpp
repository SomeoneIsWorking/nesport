#include "nesport/machine.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
std::vector<uint8_t> MakeTestRom() {
	std::vector<uint8_t> rom(16 + 0x4000 + 0x2000, 0);
	rom[0] = 'N';
	rom[1] = 'E';
	rom[2] = 'S';
	rom[3] = 0x1A;
	rom[4] = 1;
	rom[5] = 1;
	rom[16] = 0xA9; // LDA #$2A
	rom[17] = 0x2A;
	rom[18] = 0x85; // Native override replaces STA $02.
	rom[19] = 0x02;
	rom[20] = 0xEA; // NOP
	rom[21] = 0x4C; // JMP $8004
	rom[22] = 0x04;
	rom[23] = 0x80;
	for (size_t vector = 0x3FFA; vector <= 0x3FFE; vector += 2) {
		rom[16 + vector] = 0x00;
		rom[16 + vector + 1] = 0x80;
	}
	return rom;
}

void Require(bool condition, const char *message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

class RecordingLogger final : public nesport::ILogger {
  public:
	std::vector<std::string> Messages;

	void Write(nesport::LogLevel, std::string_view message) override {
		Messages.emplace_back(message);
	}
};
} // namespace

int main() {
	try {
		RecordingLogger logger;
		nesport::NativeOverrideRegistry overrides;
		nesport::RuntimeConfig invalidConfig;
		invalidConfig.InstructionBudget = 0;
		invalidConfig.UserDataDirectory = "build/test-user-data";
		bool rejectedInvalidConfig = false;
		try {
			nesport::Machine invalidMachine(invalidConfig, logger, overrides);
		} catch (const std::invalid_argument &) {
			rejectedInvalidConfig = true;
		}
		Require(rejectedInvalidConfig, "zero instruction budget was not rejected before runtime creation");

		overrides.Register({0x8000, "original-entry", "proof", [](nesport::NativeCall &) {
								return nesport::NativeOverrideAction::RunOriginal;
							}});
		overrides.Register({0x8002, "native-store", "proof", [](nesport::NativeCall &call) {
								NesPortCpuBoundaryState state = call.GetCpuState();
								const NesPortMemoryCallbacks &memory = call.Memory();
								memory.WriteData(memory.Context, 0x0002, state.Cpu.A);
								state.Cpu.PC = 0x8004;
								call.SetCpuState(state);
								return nesport::NativeOverrideAction::Handled;
							}});
		overrides.Register({0x8005, "bounded-stop", "proof", [](nesport::NativeCall &) {
								return nesport::NativeOverrideAction::Stop;
							}});

		nesport::RuntimeConfig config;
		config.InstructionBudget = 16;
		config.UserDataDirectory = "build/test-user-data";
		config.ActiveOverrideScopes = {"proof"};
		nesport::Machine machine(config, logger, overrides);
		machine.LoadRom(MakeTestRom(), "nesport-proof.nes");
		machine.WriteMemory(0x0002, 0x11);
		std::vector<uint8_t> initialState = machine.SaveState();

		nesport::RunResult result = machine.Run();
		Require(result.Stop == NesPortRunStop::HookRequested, "stop override was not reached");
		Require(result.CoreInstructions == 2, "ordinary core path count is wrong");
		Require(result.NativeInstructions == 1, "native override count is wrong");
		Require(result.ProgramCounter == 0x8005, "stop PC is wrong");
		Require(machine.ReadMemory(0x0002) == 0x2A, "native override did not write the accumulator");
		Require(logger.Messages.size() == 4, "configurable logger did not receive load and override events");

		machine.LoadState(initialState);
		Require(machine.ReadMemory(0x0002) == 0x11, "full-machine restore retained the native write");

		std::cout << "nesport runtime proof: core/original, scoped native, bounded stop, state and logger passed\n";
		return 0;
	} catch (const std::exception &error) {
		std::fprintf(stderr, "nesport runtime proof failed: %s\n", error.what());
		return 1;
	}
}
