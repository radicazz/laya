#pragma once

#include <doctest/doctest.h>
#include <laya/init.hpp>
#include <stdexcept>

namespace laya::test {

    /// RAII helper for SDL initialization in tests
    class SdlTestContext {
    public:
        SdlTestContext() {
            laya::init();
        }

        ~SdlTestContext() noexcept {
            laya::quit();
        }

        // Non-copyable, non-movable
        SdlTestContext(const SdlTestContext&) = delete;
        SdlTestContext& operator=(const SdlTestContext&) = delete;
        SdlTestContext(SdlTestContext&&) = delete;
        SdlTestContext& operator=(SdlTestContext&&) = delete;
    };

/// Helper macro for tests that need SDL initialization
#define LAYA_TEST_WITH_SDL(name)        \
    TEST_CASE(name) {                   \
        laya::test::SdlTestContext ctx; \
        (void)ctx;

    /// Helper to check if an exception is thrown
    template <typename Exception, typename Callable>
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
