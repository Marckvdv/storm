#pragma once

#include <cstdint>
#include <tuple>
#include <boost/optional.hpp>

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
            // Create ObservationsGenerator using the given sparse model and
            // policy.
            ObservationGenerator(ModelType const& model, Policy<State, Action, ValueType>& policy);

            // Generate random initial state (from the initial states
            // specified by the model)
            State randomInitialState();

            // Perform a single step in the MDP using the given action
            boost::optional<TransitionType> step(Action action);

            // Perform a single step according to action chosen by the policy
            boost::optional<TransitionType> randomStep();

            // Perform a trace using the given amount of steps. If
            // `useRandomInitialState` is false, `initialState` must be a valid
            // state in the model.
            void newTrace(int steps, bool useRandomInitialState=true, State initialState=0);

            // Generate observations
            ObservationsType generateObservations(int traces, int steps, bool useRandomInitialState=true, State initialState=0);
        private:
            State currentState;
            Observations<State, Action, Reward> observations;
            storm::utility::RandomProbabilityGenerator<ValueType> generator;
            ModelType const& model;
            Policy<State, Action, ValueType>& policy;
        };
    }
}
