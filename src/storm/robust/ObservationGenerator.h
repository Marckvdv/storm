#pragma once

#include <cstdint>
#include <tuple>
#include "storm/models/sparse/Model.h"
#include "storm/utility/random.h"
#include "storm/robust/Observations.h"
#include "storm/robust/Policy.h"

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class ObservationGenerator {
            typedef storm::models::sparse::Model<ValueType, RewardModelType> ModelType;
            typedef Transition<State, Action, Reward> TransitionType;
            typedef Observations<State, Action, Reward> ObservationsType;

        public:
            ObservationGenerator(ModelType const& model, Policy<State, Action, ValueType>& policy);
            Action randomAction();
            TransitionType step(Action action);
            TransitionType randomStep();
            void newTrace(int steps);
            ObservationsType generateObservations(int traces, int steps);
        private:
            State currentState;
            Observations<State, Action, Reward> observations;
            storm::utility::RandomProbabilityGenerator<ValueType> generator;
            ModelType const& model;
            Policy<State, Action, ValueType>& policy;
        };
    }
}
