#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nesport {
struct RuntimeConfig final {
	uint64_t InstructionBudget = 100'000;
	std::string UserDataDirectory;
	std::vector<std::string> ActiveOverrideScopes;
};
} // namespace nesport
