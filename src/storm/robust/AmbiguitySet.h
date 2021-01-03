#pragma once

#include <unordered_map>
#include <map>

#include "storm/robust/ObservationSparseModelBuilder.h"
#include "storm/robust/Observations.h"
#include "storm/robust/AmbiguityArea.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace robust {
        // Ambiguity set. Represents the intersection of the given list of
        // AmbiguityArea's
        template <typename ValueType, typename State, typename Action>
        struct AmbiguitySet {
            // Ambiguity set is a list of intersections
            std::vector<AmbiguityArea<ValueType>> intersections;
            std::map<std::pair<State, Action>, std::vector<ValueType>> constantTerm;
            std::map<std::pair<State, Action>, storm::storage::SparseMatrix<ValueType>> coefficientTerm;

            Rectangularity rectangularity;
            size_t dimensions;

            AmbiguitySet(
                    std::vector<AmbiguityArea<ValueType>> intersections = {},
                    Rectangularity rectangularity = NOT_RECTANGULAR,
                    std::map<std::pair<State, Action>, std::vector<ValueType>> constantTerm = {},
                    std::map<std::pair<State, Action>, storm::storage::SparseMatrix<ValueType>> coefficientTerm = {});
        };

        template<typename State, typename Action, typename Reward>
        class AmbiguitySetBuilder : public ObservationSparseModelBuilder<State, Action, Reward> {
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;
            typedef storm::storage::SparseMatrix<storm::RationalNumber> TransitionMatrix;
        public:
            AmbiguitySetBuilder(Observations<State, Action, Reward> observations);
            AmbiguitySet<storm::RationalNumber, State, Action> calculateAmbiguitySet();
            AmbiguitySet<storm::RationalNumber, State, Action> calculateAmbiguitySet(TransitionsMap transitions);
        };
    }
}
