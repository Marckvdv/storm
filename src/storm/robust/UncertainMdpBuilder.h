#pragma once

#include "Observations.h"

#include "storm/storage/jani/Model.h"
#include "storm-pars/storage/ParameterRegion.h"

#include <map>
#include <vector>
#include <utility>
#include <iostream>

namespace storm {
    namespace robust {
        // Transition information as described in:
        // "Interval change-point detection for runtime probabilistic model
        // checking"
        template <typename ValueType>
        struct TransitionInformation {
            std::pair<ValueType, ValueType> transitionInterval;
            std::pair<ValueType, ValueType> sampleInterval;
            std::pair<ValueType, ValueType> branchTaken;

            TransitionInformation();
            TransitionInformation(
                    std::pair<ValueType, ValueType> transitionInterval,
                    std::pair<ValueType, ValueType> sampleInterval);

            // Update current transition information with the given new
            // observations
            void updateInformation(ValueType part, ValueType total);
        };

        template <typename State, typename Action, typename Reward, typename ValueType>
        class UncertainMdpBuilder {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;
            typedef std::map<std::tuple<State, Action, State>, TransitionInformation<ValueType>> PriorType;

        public:
            UncertainMdpBuilder();

            // Update current transition information with the given observations
            void processObservations(Observations<State, Action, Reward> const& observations);

            // Load a prior form a file
            void loadPrior(std::string path);

            // Directly load a prior
            void loadPrior(PriorType prior);

            // Export the current intervals to the given output stream
            void exportIntervals(std::ostream& output);

            // Build a MDP with the current information
            storm::jani::Model buildMdp();
        private:
            TransitionsMap calculateTransitionsMap(Observations<State, Action, Reward> const& observations);

            PriorType info;
        };
    }
}
