#pragma once

#include "storm/utility/random.h"

namespace storm {
    namespace robust {
        // Generic policy interface, supporting both stationary and
        // non-stationary policies
        template <typename State, typename Action, typename ValueType>
        class Policy {
        public:
            virtual void addHistory(State state) = 0;
            virtual Action getNextAction(
                    storm::utility::RandomProbabilityGenerator<ValueType> gen,
                    State state) = 0;
        };
    }
}