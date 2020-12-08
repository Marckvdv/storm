#include "UniformPolicy.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        UniformPolicy<State, Action, ValueType>::UniformPolicy(std::vector<Action> actions) : actions(actions) {
            STORM_LOG_ASSERT(actions.size() > 0, "Empty action set");
        }

        template <typename State, typename Action, typename ValueType>
        void UniformPolicy<State, Action, ValueType>::addHistory(State state) {
            // Intentionally left empty
        }

        template <typename State, typename Action, typename ValueType>
        Action UniformPolicy<State, Action, ValueType>::getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state) {
            uint64_t index = gen.random_uint(0, actions.size() - 1);
            return actions[index];
        }

        template class UniformPolicy<uint64_t, uint64_t, double>;
        template class UniformPolicy<uint64_t, uint64_t, storm::RationalNumber>;
        template class UniformPolicy<uint64_t, uint64_t, storm::RationalFunction>;
    }
}
