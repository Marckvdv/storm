#include "StationaryPolicy.h"

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/io/file.h"

#include <string>

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename ValueType>
        StationaryPolicy<State, Action, ValueType>::StationaryPolicy(std::unordered_map<State, std::unordered_map<Action, ValueType>> policyData) :
            policyData(policyData) {
            // Intentionally left empty
        }

        template <typename State, typename Action, typename ValueType>
        StationaryPolicy<State, Action, ValueType>::StationaryPolicy(storm::json<double> data) : policyData({}) {
            initFromJson(data);
        }

        template <typename State, typename Action, typename ValueType>
        StationaryPolicy<State, Action, ValueType>::StationaryPolicy(std::string path) : policyData({}) {
            storm::json<double> j;
            std::ifstream file;
            storm::utility::openFile(path, file);
            j << file;
            initFromJson(j);
        }

        template <typename State, typename Action, typename ValueType>
        void StationaryPolicy<State, Action, ValueType>::initFromJson(storm::json<double> data) {
            for (auto kv = data.begin(); kv != data.end(); ++kv) {
                std::string s1s = kv.key();
                std::cout << "Got here, k: " << kv.key() << " v: " << kv.value() << std::endl;
                State s1 = std::stoi(s1s);
                bool first = true;
                auto inner = kv.value();
                for (auto kv2 = inner.begin(); kv2 != inner.end(); ++kv2) {
                    std::string as = kv2.key();
                    std::cout << "and Got here" << std::endl;
                    Action a = std::stoi(as);
                    double prob = kv2.value().front();

                    if (first) {
                        policyData[s1] = {};
                        first = false;
                    }

                    policyData[s1][a] = ValueType(prob);
                }
            }
        }

        template <typename State, typename Action, typename ValueType>
        void StationaryPolicy<State, Action, ValueType>::addHistory(State state) {
            // Intentionally left empty
        }

        template <typename State, typename Action, typename ValueType>
        Action StationaryPolicy<State, Action, ValueType>::getNextAction(storm::utility::RandomProbabilityGenerator<ValueType> gen, State state) {
            ValueType probability = gen.random();
            ValueType sum = storm::utility::zero<ValueType>();

            for (auto const& entry : policyData[state]) {
                sum += entry.second;

                if (probability < sum) {
                    return entry.first;
                }
            }

            STORM_LOG_ERROR("Policy is not probabilistic or incomplete");
        }

        template class StationaryPolicy<uint64_t, uint64_t, double>;
        template class StationaryPolicy<uint64_t, uint64_t, storm::RationalNumber>;
        template class StationaryPolicy<uint64_t, uint64_t, storm::RationalFunction>;
    }
}
