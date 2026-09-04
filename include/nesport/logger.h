#pragma once

#include <string_view>

namespace nesport {
enum class LogLevel {
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
	void Write(LogLevel, std::string_view) override {}
};
} // namespace nesport
