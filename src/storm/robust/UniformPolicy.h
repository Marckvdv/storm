#pragma once

#include "Policy.h"
#include "storm/models/sparse/Model.h"

namespace storm {
    namespace robust {
        // A 'uniform' policy that acts as a default policy when no policy is
        // specified. Chooses a random action from the list of available
        // actions, all with equal probability
        template<typename State, typename Action, typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class UniformPolicy : public Policy<State, Action, ValueType> {
            typedef storm::models::sparse::Model<ValueType, RewardModelType> ModelType;
        public:
            // Construct a uniform policy with the given available actions
            UniformPolicy(ModelType const& model);

            // Override
            void addHistory(State state);
            Action getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state);
        private:
            ModelType const& model;
        };
    }
}
