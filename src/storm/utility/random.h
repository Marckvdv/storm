#pragma once

#include <random>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace utility {
        template<typename ValueType>
        class RandomProbabilityGenerator {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            ValueType random() const;
            uint64_t random_uint(uint64_t min, uint64_t max);

        };

        template<>
        class RandomProbabilityGenerator<double> {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            double random();
            uint64_t random_uint(uint64_t min, uint64_t max);
        private:
            std::uniform_real_distribution<double> distribution;
            std::mt19937 engine;

        };

        template<>
        class RandomProbabilityGenerator<storm::RationalNumber> {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            storm::RationalNumber random();
            uint64_t random_uint(uint64_t min, uint64_t max);
        private:
            std::uniform_real_distribution<double> distribution;
            std::mt19937 engine;

        };

        template<>
        class RandomProbabilityGenerator<storm::RationalFunction> {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            storm::RationalFunction random();
            uint64_t random_uint(uint64_t min, uint64_t max);
        private:
            std::uniform_real_distribution<double> distribution;
            std::mt19937 engine;

        };
    }
}
