#pragma once

#include "storm/utility/random.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        class Policy {
        public:
            virtual void addHistory(State state) = 0;
            virtual Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state) = 0;
        };
    }
}
