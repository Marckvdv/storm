#pragma once

#include "Observations.h"

#include "storm/storage/jani/Model.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename Reward>
        class JaniFromObservationsBuilder {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;

        public:
            JaniFromObservationsBuilder(Observations<State, Action, Reward> observations);
            storm::jani::Model build();

        protected:
            TransitionsMap calculateTransitionsMap(State& highestState, Action& highestAction);
        private:
            Observations<State, Action, Reward> observations;
        };

    }
}
