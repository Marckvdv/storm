#include "storm/robust/AmbiguityArea.h"

#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
    namespace robust {
        template <typename ValueType>
        AmbiguityArea<ValueType>::AmbiguityArea(storm::storage::SparseMatrix<ValueType> matrix, std::vector<ValueType> vector, ValueType scalar) :
            matrix(matrix), vector(vector), scalar(scalar) {
                // Intentionally left empty
        }

        template class AmbiguityArea<storm::RationalNumber>;
    }
}
