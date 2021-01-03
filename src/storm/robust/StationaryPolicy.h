#pragma once

#include "Policy.h"
#include "storm/adapters/JsonAdapter.h"

#include <unordered_map>

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        class StationaryPolicy : public Policy<State, Action, ValueType> {
            typedef std::unordered_map<State, std::unordered_map<Action, ValueType>> PolicyData;

        public:
            // Initialize from given policy data
            StationaryPolicy(PolicyData policy);

            // Initialize from policy data given in JSON
            StationaryPolicy(storm::json<double> data);

            // Initialize from policy data given in JSON in the given file
            StationaryPolicy(std::string path);

            // Helper initialize function
            void initFromJson(storm::json<double> data);

            // Override Policy interface functions
            void addHistory(State state);
            Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state);
        private:
            PolicyData policyData;
        };
    }
}
