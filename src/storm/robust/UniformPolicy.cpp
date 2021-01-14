#include "UniformPolicy.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename ValueType, typename RewardModelType>
        UniformPolicy<State, Action, ValueType, RewardModelType>::UniformPolicy(storm::models::sparse::Model<ValueType, RewardModelType> const& model) : model(model) {
            // Intentionally left empty
        }

        template<typename State, typename Action, typename ValueType, typename RewardModelType>
        void UniformPolicy<State, Action, ValueType, RewardModelType>::addHistory(State state) {
            // Intentionally left empty
        }

        template<typename State, typename Action, typename ValueType, typename RewardModelType>
        Action UniformPolicy<State, Action, ValueType, RewardModelType>::getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state) {
            auto transitions = model.getTransitionMatrix();
            auto indices = transitions.getRowGroupIndices();
            auto numActions = indices[state+1] - indices[state];
            auto action = gen.random_uint(0, numActions-1);
            return action;
        }

        template class UniformPolicy<uint64_t, uint64_t, double, storm::models::sparse::StandardRewardModel<double>>;
        template class UniformPolicy<uint64_t, uint64_t, storm::RationalNumber, storm::models::sparse::StandardRewardModel<storm::RationalNumber>>;
        template class UniformPolicy<uint64_t, uint64_t, storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>;
    }
}
