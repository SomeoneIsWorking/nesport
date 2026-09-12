#ifndef NESPORT_LOGGER_H
#define NESPORT_LOGGER_H

#include <cstdint>
#include <string_view>

namespace nesport {
enum class LogLevel : std::uint8_t {
	Debug,
	Info,
	Warning,
	Error,
};

class ILogger {
  public:
	virtual ~ILogger() = default;
	virtual void Write(LogLevel level, std::string_view message) = 0;
};

class NullLogger final : public ILogger {
  public:
	void Write(LogLevel, std::string_view) override {
	}
};
} // namespace nesport

#endif // NESPORT_LOGGER_H
