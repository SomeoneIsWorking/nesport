#pragma once

#include "NES/Port/NesPortExecution.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace nesport {
enum class NativeOverrideAction {
	Handled,
	RunOriginal,
	Stop,
};

class NativeCall final {
  public:
	explicit NativeCall(NesPortInstructionContext &context) : _context(context) {}

	NesPortCpuBoundaryState GetCpuState() const { return _context.GetCpuState(); }
	void SetCpuState(const NesPortCpuBoundaryState &state) { _context.SetCpuState(state); }
	const NesPortMemoryCallbacks &Memory() const { return _context.GetMemoryCallbacks(); }

  private:
	NesPortInstructionContext &_context;
};

using NativeOverrideHandler = std::function<NativeOverrideAction(NativeCall &)>;

struct NativeOverride final {
	uint16_t GuestAddress = 0;
	std::string Name;
	std::string Scope;
	NativeOverrideHandler Handler;
};

class NativeOverrideRegistry final {
  public:
	void Register(NativeOverride nativeOverride);
	const NativeOverride *Find(uint16_t guestAddress, const std::vector<std::string> &activeScopes) const;

  private:
	std::unordered_map<uint16_t, std::vector<NativeOverride>> _byAddress;
};
} // namespace nesport
