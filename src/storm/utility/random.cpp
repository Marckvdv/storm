#include "storm/utility/random.h"

namespace storm {
    namespace utility {
        // double RandomProbabilityGenerator
        RandomProbabilityGenerator<double>::RandomProbabilityGenerator()
        : distribution(0.0, 1.0)
        {
            std::random_device rd;
            engine = std::mt19937(rd());
        }

        RandomProbabilityGenerator<double>::RandomProbabilityGenerator(uint64_t seed)
        : distribution(0.0, 1.0), engine(seed)
        {

        }

        double RandomProbabilityGenerator<double>::random() {
            return distribution(engine);
        }

        uint64_t RandomProbabilityGenerator<double>::random_uint(uint64_t min, uint64_t max) {
            return std::uniform_int_distribution<uint64_t>(min, max)(engine);
        }

        // RationalNumber RandomProbabilityGenerator
        RandomProbabilityGenerator<storm::RationalNumber>::RandomProbabilityGenerator()
        : distribution(0.0, 1.0)
        {
            std::random_device rd;
            engine = std::mt19937(rd());
        }

        RandomProbabilityGenerator<storm::RationalNumber>::RandomProbabilityGenerator(uint64_t seed)
        : distribution(0.0, 1.0), engine(seed)
        {

        }

        storm::RationalNumber RandomProbabilityGenerator<storm::RationalNumber>::random() {
            uint64_t precision = 1;
            precision <<= 32;
            uint64_t numerator = random_uint(0, precision);

            return storm::RationalNumber(numerator, precision);
        }

        uint64_t RandomProbabilityGenerator<storm::RationalNumber>::random_uint(uint64_t min, uint64_t max) {
            return std::uniform_int_distribution<uint64_t>(min, max)(engine);
        }


        // RationalFunction RandomProbabilityGenerator
        RandomProbabilityGenerator<storm::RationalFunction>::RandomProbabilityGenerator()
        : distribution(0.0, 1.0)
        {
            std::random_device rd;
            engine = std::mt19937(rd());
        }

        RandomProbabilityGenerator<storm::RationalFunction>::RandomProbabilityGenerator(uint64_t seed)
        : distribution(0.0, 1.0), engine(seed)
        {

        }

        storm::RationalFunction RandomProbabilityGenerator<storm::RationalFunction>::random() {
            uint64_t precision = 1;
            precision <<= 32;
            uint64_t numerator = random_uint(0, precision);

            return storm::RationalFunction(numerator) / precision;
        }

        uint64_t RandomProbabilityGenerator<storm::RationalFunction>::random_uint(uint64_t min, uint64_t max) {
            return std::uniform_int_distribution<uint64_t>(min, max)(engine);
        }
    }
}
