#pragma once

#include <stdexcept>

namespace ext_sort::tape {

class TapeException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class EndOfTapeException : public TapeException {
 public:
  EndOfTapeException() : TapeException("end of tape") {}
};

class BeginOfTapeException : public TapeException {
 public:
  BeginOfTapeException() : TapeException("begin of tape") {}
};

class TapeIoException : public TapeException {
 public:
  using TapeException::TapeException;
};

}  // namespace ext_sort::tape
