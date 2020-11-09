#pragma once

#include "Observations.h"

#include "storm/storage/jani/Model.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename Reward>
        class JaniRegionFromObservationsBuilder {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;

        public:
            JaniRegionFromObservationsBuilder(Observations<State, Action, Reward> observations);
            std::pair<storm::jani::Model, storm::storage::ParameterRegion<storm::RationalFunction>> build();
            std::pair<double, double> calculateLowerUpperBound(uint64_t part, uint64_t total, double confidence);

        protected:
            TransitionsMap calculateTransitionsMap(State& highestState, Action& highestAction);
        private:
            Observations<State, Action, Reward> observations;
        };

    }
}
