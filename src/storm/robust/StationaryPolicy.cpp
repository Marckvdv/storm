#include "StationaryPolicy.h"

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        StationaryPolicy<State, Action, ValueType>::StationaryPolicy(std::unordered_map<State, std::unordered_map<Action, ValueType>> policyData) :
            policyData(policyData) {
            // Intentionally left empty
        }

        template <typename State, typename Action, typename ValueType>
        void StationaryPolicy<State, Action, ValueType>::addHistory(State state) {
            // Intentionally left empty
        }

        template <typename State, typename Action, typename ValueType>
        Action StationaryPolicy<State, Action, ValueType>::getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen) {
            ValueType probability = gen.random();
            ValueType sum = storm::utility::zero<ValueType>();

            for (auto const& entry : policyData) {
                sum += entry.second;

                if (probability < sum) {
                    return entry.first;
                }
            }

            STORM_LOG_ERROR("Policy is not probabilistic");
        }
    }
}
