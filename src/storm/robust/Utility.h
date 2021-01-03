#pragma once

// Utility header for common things

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace robust {
        // Helper function to convert the most common ValueType's to a double
        template <typename ValueType>
        double toDouble(ValueType value);

        // Double to double is the identity function
        template <>
        double toDouble(double value) {
            return value;
        }

        // RationalNumber to double. Implementation depends on the chosen
        // implementation of rational numbers.
        template <>
        double toDouble(storm::RationalNumber value) {
#if defined(STORM_HAVE_CLN) && defined(STORM_USE_CLN_EA)
            return cln::double_approx(value);
#elif defined(STORM_HAVE_GMP) && !defined(STORM_USE_CLN_EA)
            return value.get_d();
#endif
        }

        // RationalFunction to double
        template <>
        double toDouble(storm::RationalFunction value) {
            return cln::double_approx(value.nominatorAsNumber() / value.denominatorAsNumber());
        }
    }
}
