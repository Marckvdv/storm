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
            StationaryPolicy(PolicyData policy);
            StationaryPolicy(storm::json<double> data);
            StationaryPolicy(std::string path);

            void initFromJson(storm::json<double> data);

            // Override
            void addHistory(State state);
            Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state);
        private:
            PolicyData policyData;
        };
    }
}
