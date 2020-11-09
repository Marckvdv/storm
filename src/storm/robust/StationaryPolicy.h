#pragma once

#include "Policy.h"

#include <unordered_map>

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        class StationaryPolicy : public Policy<State, Action, ValueType> {
            typedef std::unordered_map<State, std::unordered_map<Action, ValueType>> PolicyData;

        public:
            StationaryPolicy(PolicyData policy);

            // Override
            void addHistory(State state);
            Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen);
        private:
            PolicyData policyData;
        };
    }
}
