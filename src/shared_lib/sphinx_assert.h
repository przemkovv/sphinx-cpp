
#pragma once

#ifndef SPHINX_NDEBUG

#include "Logger.h"
#include <exception>
#include <fmt/format.h>
#include <string>

#define THROWASSERT_LOGGER(MESSAGE)                                            \
  {                                                                            \
    Sphinx::make_logger("Assert")->error(MESSAGE);                             \
  }

/// Exception type for assertion failures
class AssertionFailureException : public std::exception {
private:
  const char *expression;
  const char *file;
  int line;
  std::string message;
  std::string report;

public:
  /// Log error before throwing
  void LogError()
  {
#ifdef THROWASSERT_LOGGER
    THROWASSERT_LOGGER(report);
#endif
  }

  /// Construct an assertion failure exception
  AssertionFailureException(const char *expression,
                            const char *file,
                            int line,
                            const std::string &message)
    : expression(expression), file(file), line(line), message(message)
  {
    fmt::MemoryWriter outputStream;

    if (!message.empty()) {
      outputStream << fmt::format("{}: ", message);
    }

    std::string expressionString = expression;
    if (expressionString == "false" || expressionString == "0" ||
        expressionString == "FALSE") {
      outputStream << "Unreachable code assertion";
    }
    else {
      outputStream << fmt::format("Assertion '{}'", expression);
    }

    outputStream << fmt::format(" failed in file '{}' line {}", file, line);
    report = outputStream.str();

    LogError();
  }

  /// The assertion message
  virtual const char *what() const noexcept(true) { return report.c_str(); }

  /// The expression which was asserted to be true
  const char *Expression() const { return expression; }

  /// Source file
  const char *File() const { return file; }

  /// Source line
  int Line() const { return line; }

  /// Description of failure
  const char *Message() const { return message.c_str(); }
};

/// Assert that EXPRESSION evaluates to true, otherwise raise
/// AssertionFailureException with associated MESSAGE (which may use C++
/// stream-style message formatting)
#define SPHINX_ASSERT(EXPRESSION, ...)                                         \
  if (!(EXPRESSION)) {                                                         \
    throw AssertionFailureException(#EXPRESSION, __FILE__, __LINE__,           \
                                    (fmt::format(__VA_ARGS__)));               \
  }

#endif