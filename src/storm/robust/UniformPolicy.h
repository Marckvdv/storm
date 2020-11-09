#pragma once

#include "Policy.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        class UniformPolicy : public Policy<State, Action, ValueType> {
        public:
            UniformPolicy(std::vector<Action> actions);

            // Override
            void addHistory(State state);
            Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen);
        private:
            std::vector<Action> actions;
        };
    }
}
