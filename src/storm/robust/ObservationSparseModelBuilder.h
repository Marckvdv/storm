#pragma once

#include <map>

#include "storm/robust/Observations.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward>
        class ObservationSparseModelBuilder {
            typedef Observations<State, Action, Reward> ObservationsType;
            typedef std::map<State, std::map<Action, std::map<State, uint64_t>>> TransitionsMap;
            typedef storm::storage::SparseMatrix<storm::RationalNumber> TransitionMatrix;
            typedef storm::models::sparse::Mdp<storm::RationalNumber> Mdp;

            ObservationsType observations;

        protected:
            TransitionsMap calculateTransitionsMap(State& highestState, Action& highestAction);

        public:
            ObservationSparseModelBuilder(ObservationsType observations);
            Mdp buildMdp();

        private:
            void printTransitions(TransitionsMap const& transitions);

            template<typename V>
            void printSparseMatrix(storm::storage::SparseMatrix<V> const& matrix) {
                int rowCount = matrix.getRowCount();
                for (int row = 0; row < rowCount; ++row) {
                    for (auto const& entry : matrix.getRow(row)) {
                        std::cout << "matrix[" << row << "," << entry.getColumn() << "] = " << entry.getValue() << std::endl;
                    }
                }
            }
        };
    }
}
