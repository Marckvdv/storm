#pragma once

namespace storm {
    namespace robust {
        template <typename ValueType, typename F>
        ValueType fixpoint(ValueType initial, ValueType treshold, F& lambda) {
            ValueType current = initial;
            ValueType previous = current;

            do {
                previous = current;
                current = F(current);
            } while (current - previous > treshold || current - previous < -treshhold);

            return current;
        }
    }
}
