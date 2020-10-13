#pragma once

#include <unordered_map>
#include <map>

#include "storm/robust/ObservationSparseModelBuilder.h"
#include "storm/robust/Observations.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace robust {
        class AmbiguitySet {
        };

        template<typename State, typename Action, typename Reward>
        class AmbiguitySetBuilder : public ObservationSparseModelBuilder<State, Action, Reward> {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;
            typedef storm::storage::SparseMatrix<storm::RationalNumber> TransitionMatrix;
        public:
            AmbiguitySetBuilder(Observations<State, Action, Reward> observations);
            void calculateAmbiguitySet();
            void calculateAmbiguitySet(TransitionsMap transitions);
        };
    }
}
