#pragma once

#include "Observations.h"

#include "storm/storage/jani/Model.h"
#include "storm-pars/storage/ParameterRegion.h"

#include <map>
#include <vector>
#include <utility>

namespace storm {
    namespace robust {
        template <typename ValueType>
        struct TransitionInformation {
            std::pair<ValueType, ValueType> transitionInterval;
            std::pair<ValueType, ValueType> sampleInterval;

            TransitionInformation();
            TransitionInformation(std::pair<ValueType, ValueType> transitionInterval, std::pair<ValueType, ValueType> sampleInterval);
            void updateInformation(ValueType part, ValueType total);
        };

        template <typename State, typename Action, typename Reward, typename ValueType>
        class UncertainMdpBuilder {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;

        public:
            UncertainMdpBuilder();
            void processObservations(Observations<State, Action, Reward> const& observations);
            storm::jani::Model buildMdp();
        private:
            TransitionsMap calculateTransitionsMap(Observations<State, Action, Reward> const& observations);

            std::map<std::tuple<State, Action, State>, TransitionInformation<ValueType>> info;
        };
    }
}
