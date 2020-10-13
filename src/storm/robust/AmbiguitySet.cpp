#include "storm/robust/AmbiguitySet.h"

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward>
        AmbiguitySetBuilder<State, Action, Reward>::AmbiguitySetBuilder(Observations<State, Action, Reward> observations)
            : ObservationSparseModelBuilder<State, Action, Reward> { observations } {
        }

        template<typename State, typename Action, typename Reward>
        void AmbiguitySetBuilder<State, Action, Reward>::calculateAmbiguitySet() {
            State s;
            Action a;
            auto transitions = ObservationSparseModelBuilder<State, Action, Reward>::calculateTransitionsMap(s, a);
            calculateAmbiguitySet(transitions);
        }

        template<typename State, typename Action, typename Reward>
        struct cmp {
            bool operator()(const std::tuple<State, Action, Reward>& a, const std::tuple<State, Action, Reward>& b) const {
                return a <= b;
            }
        };

        template<typename State, typename Action, typename Reward>
        void AmbiguitySetBuilder<State, Action, Reward>::calculateAmbiguitySet(TransitionsMap transitions) {
            typedef std::tuple<State, Action, State> T;
            // 1+2] Calculate \mu and \xi
            auto comp = [](const T& a, const T& b) { return a <= b; };

            std::map<T, size_t, decltype(comp)> index_map(comp);
            std::vector<storm::RationalNumber> maximum_likelihood;
            size_t current_index = 0;
            for (auto const& t1 : transitions) {
                auto s1 = t1.first;
                auto& v1 = t1.second;
                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;
                    bool first = true;

                    auto total = 0;
                    for (auto const& t3 : v2) {
                        total += t3.second;
                    }

                    for (auto const& t3 : v2) {
                        if (first) {
                            first = false;
                            continue;
                        }

                        auto s2 = t3.first;
                        auto amount = t3.second;

                        if (amount > 0) {
                            index_map.insert({ std::make_tuple(s1, action, s2), current_index });
                            ++current_index;
                            maximum_likelihood.push_back(storm::RationalNumber(amount) / total);
                        }
                    }
                }
            }
            // 1.1] Calculate q (= len(\mu))
            size_t q = index_map.size();
            std::cout << "q=" << q << std::endl;

            // 3] Structural ambiguity set

            // 4] Quadratic approx
        }

        template class AmbiguitySetBuilder<uint64_t, uint64_t, double>;
        template class AmbiguitySetBuilder<uint64_t, uint64_t, storm::RationalNumber>;
        template class AmbiguitySetBuilder<uint64_t, uint64_t, storm::RationalFunction>;
    }
}
