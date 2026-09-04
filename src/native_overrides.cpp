#include "nesport/native_overrides.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace nesport {
void NativeOverrideRegistry::Register(NativeOverride nativeOverride) {
	if (nativeOverride.Name.empty()) {
		throw std::invalid_argument("native override name must not be empty");
	}
	if (!nativeOverride.Handler) {
		throw std::invalid_argument("native override handler must be callable");
	}
	auto &entries = _byAddress[nativeOverride.GuestAddress];
	auto duplicate =
		std::find_if(entries.begin(), entries.end(), [&](const NativeOverride &entry) { return entry.Scope == nativeOverride.Scope; });
	if (duplicate != entries.end()) {
		throw std::invalid_argument("guest address and scope already have a native override");
	}
	entries.push_back(std::move(nativeOverride));
}

const NativeOverride *NativeOverrideRegistry::Find(uint16_t guestAddress, const std::vector<std::string> &activeScopes) const {
	auto found = _byAddress.find(guestAddress);
	if (found == _byAddress.end()) {
		return nullptr;
	}
	for (const NativeOverride &entry : found->second) {
		if (entry.Scope.empty() || std::find(activeScopes.begin(), activeScopes.end(), entry.Scope) != activeScopes.end()) {
			return &entry;
		}
	}
	return nullptr;
}
} // namespace nesport
