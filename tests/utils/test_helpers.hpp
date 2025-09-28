#pragma once

#include <stdexcept>

#include <doctest/doctest.h>
#include <laya/laya.hpp>

namespace laya_test {

/// Helper macro for tests that need SDL initialization
#define LAYA_TEST_WITH_SDL(name) \
    TEST_CASE(name) {            \
        [[maybe_unused]] laya::context ctx(laya::subsystem::video);

/// Helper to check if an exception is thrown
template <class Exception, class Callable>
bool throws_exception(Callable&& callable) {
    try {
        callable();
        return false;
    } catch (const Exception&) {
        return true;
    } catch (...) {
        return false;
    }
}

}  // namespace laya::test
