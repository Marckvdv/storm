#include "ObservationsParser.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "storm/io/file.h"

namespace storm {
    namespace robust {
        template <typename ValueType>
        Observations<uint64_t, uint64_t, ValueType> ObservationsParser<ValueType>::parse(std::string const& path) {
            ObservationsParser parser;
            parser.readFile(path);
            return parser.parseObservations();
        }

        template <typename ValueType>
        ObservationsParser<ValueType>::ObservationsParser() {
        }

        template <typename ValueType>
        ObservationsParser<ValueType>::ObservationsParser(std::string const& jsonstring) {
            parsedStructure = Json::parse(jsonstring);
        }

        template <typename ValueType>
        void ObservationsParser<ValueType>::readFile(std::string const &path) {
            std::ifstream file;
            storm::utility::openFile(path, file);
            parsedStructure << file;
            storm::utility::closeFile(file);
        }

        template <typename ValueType>
        Observations<uint64_t, uint64_t, ValueType> ObservationsParser<ValueType>::parseObservations() {
            ObservationsType observations;

            typedef std::unordered_map<std::string, uint64_t> index_map;

            index_map state_indices;
            index_map action_indices;

            auto lookup_index = [] (auto key, auto& map) {
                auto search = map.find(key);
                if (search == map.end()) {
                    uint64_t index = map.size();
                    map[key] = index;
                    return index;
                } else {
                    return search->second;
                }
            };

            auto lookup_state = [&state_indices, lookup_index] (auto key) { return lookup_index(key, state_indices); };
            auto lookup_action = [&action_indices, lookup_index] (auto key) { return lookup_index(key, action_indices); };

            for (auto const& trace : parsedStructure) {
                uint64_t initial = lookup_state(trace.at("initial").front());
                Trace<uint64_t, uint64_t, ValueType> newTrace(initial);
                for (auto const& transition : trace.at("transitions")) {
                    uint64_t action = lookup_action(transition[0].front());
                    double reward = transition[1].front();
                    uint64_t next_state = lookup_state(transition[2].front());

                    Transition<uint64_t, uint64_t, ValueType> newTransition(next_state, action, reward);
                    newTrace.addTransition(newTransition);
                }
                observations.addTrace(newTrace);
            }
            std::cout << "Read the following observations:" << std::endl;
            observations.writeToFile(std::cout);

            return observations;
        }

        template class ObservationsParser<double>;
        /*
        template class ObservationsParser<storm::RationalNumber>;
        */
    }
}
