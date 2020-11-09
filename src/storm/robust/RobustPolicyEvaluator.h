#pragma once

#include "storm/robust/AmbiguitySet.h"
#include "gurobi_c++.h"

namespace storm {
    namespace robust {
        template <typename ValueType, typename State, typename Action>
        class RobustPolicyEvaluator {
            AmbiguitySet<ValueType, State, Action> ambiguitySet;
        public:
            RobustPolicyEvaluator(AmbiguitySet<ValueType, State, Action> ambiguitySet);

            ValueType estimateWorstCaseReward();
        };
    }
}
