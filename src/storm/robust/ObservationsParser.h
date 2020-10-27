#pragma once
#include "storm/adapters/JsonAdapter.h"
#include "storm/robust/Observations.h"

namespace storm {
    namespace robust {
        /*
         * The observations format parser.
         * Parses Models and Properties
         */
        template <typename ValueType>
        class ObservationsParser {
        public:
            typedef storm::json<ValueType> Json;
            typedef Observations<uint64_t, uint64_t, ValueType> ObservationsType;

            ObservationsParser();
            ObservationsParser(std::string const& jsonstring);
            static ObservationsType parse(std::string const& path);

        protected:
            void readFile(std::string const& path);
            ObservationsType parseObservations();

        private:
            /**
             * The overall structure currently under inspection.
             */
            Json parsedStructure;
            ObservationsType observations;
        };
    }
}

