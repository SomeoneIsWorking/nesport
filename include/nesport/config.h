#ifndef NESPORT_CONFIG_H
#define NESPORT_CONFIG_H

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

#endif // NESPORT_CONFIG_H
