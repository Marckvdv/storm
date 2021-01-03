#pragma once

#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace robust {
        // Types of rectangularity
        enum Rectangularity {
            NOT_RECTANGULAR,
            S_RECTANGULAR,
            SA_RECTANGULAR
        };

        // AmbiguityArea type
        // Represents the area given by the closed halfspace/ellipsoid:
        // x^T * matrix * x + vector^T * x + scalar >= 0
        // where * is matrix/vector multiplication
        template <typename ValueType>
        struct AmbiguityArea {
            typedef storm::storage::SparseMatrix<ValueType> MatrixType;
            typedef std::vector<ValueType> VectorType;

            MatrixType matrix;
            VectorType vector;
            ValueType scalar;

        public:
            AmbiguityArea(MatrixType matrix, VectorType vector = {}, ValueType scalar = 0);
        };
    }
}
