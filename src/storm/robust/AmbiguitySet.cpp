#include "storm/robust/AmbiguitySet.h"

#include "storm/robust/AmbiguityArea.h"
#include "storm/utility/constants.h"

// Create a zero matrix with `rows` rows and `columns` columns
template <typename ValueType>
storm::storage::SparseMatrix<ValueType> zeroMatrix(size_t rows, size_t columns) {
    storm::storage::SparseMatrixBuilder<ValueType> builder(rows, columns);
    return builder.build();
}


// Create a zero vector of dimension `rows`
template <typename ValueType>
std::vector<ValueType> zeroVector(size_t rows) {
    return std::vector<ValueType>(rows, storm::utility::zero<ValueType>());
}

// Create a vector with a single value set to 1 (and the rest to 0)
template <typename ValueType>
std::vector<ValueType> oneVector(size_t rows, size_t i) {
    std::vector<ValueType> v(rows, storm::utility::zero<ValueType>());
    v[i] = storm::utility::one<ValueType>();
    return v;
}

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward>
        AmbiguitySetBuilder<State, Action, Reward>::AmbiguitySetBuilder(Observations<State, Action, Reward> observations)
            : ObservationSparseModelBuilder<State, Action, Reward> { observations } {
        }

        template<typename State, typename Action, typename Reward>
        AmbiguitySet<storm::RationalNumber, State, Action> AmbiguitySetBuilder<State, Action, Reward>::calculateAmbiguitySet() {
            State s;
            Action a;
            auto transitions = ObservationSparseModelBuilder<State, Action, Reward>::calculateTransitionsMap(s, a);
            return calculateAmbiguitySet(transitions);
        }

        template<typename State, typename Action, typename Reward>
        AmbiguitySet<storm::RationalNumber, State, Action> AmbiguitySetBuilder<State, Action, Reward>::calculateAmbiguitySet(TransitionsMap transitions) {
            typedef std::tuple<State, Action, State> T;
            // 1+2] Calculate \mu and \xi
            auto comp = [](const T& a, const T& b) { return a <= b; };

            // Construct index mapping, assigning a unique index for each
            // transition triplet.
            std::map<T, size_t, decltype(comp)> index_map(comp);
            std::vector<storm::RationalNumber> maximum_likelihood;
            size_t current_index = 0;
            // Loop over all transitions in the transition map
            for (auto const& t1 : transitions) {
                auto s1 = t1.first;
                auto& v1 = t1.second;
                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;

                    auto total = 0;
                    for (auto const& t3 : v2) {
                        total += t3.second;
                    }

                    bool first = true;
                    for (auto const& t3 : v2) {
                        // Skip first transition as it is defined in terms of
                        // the other probabilities
                        if (first) {
                            first = false;
                            continue;
                        }

                        auto s2 = t3.first;
                        auto amount = t3.second;

                        if (amount > 0) {
                            // Add triplet to index map
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
            std::vector<AmbiguityArea<storm::RationalNumber>> intersections;
            // >= 0
            for (size_t i = 0; i < q; ++i) {
                AmbiguityArea<storm::RationalNumber> positive(zeroMatrix<storm::RationalNumber>(q, q), oneVector<storm::RationalNumber>(q, i), 0);
                intersections.push_back(positive);
            }

            for (auto const& t1 : transitions) {
                auto s1 = t1.first;
                auto& v1 = t1.second;
                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;

                    auto vec = zeroVector<storm::RationalNumber>(q);
                    bool first = true;
                    for (auto const& t3 : v2) {
                        if (first) {
                            first = false;
                            continue;
                        }

                        auto s2 = t3.first;
                        auto amount = t3.second;

                        vec[index_map[std::make_tuple(s1, action, s2)]] = storm::utility::one<storm::RationalNumber>();
                    }

                    // Push ambiguity set constraint
                    AmbiguityArea<storm::RationalNumber> area(zeroMatrix<storm::RationalNumber>(q, q), vec, 0);
                    intersections.push_back(area);
                }
            }


            // 4] Quadratic approx

            return AmbiguitySet<storm::RationalNumber, State, Action>(intersections);
        }

        template <typename ValueType, typename State, typename Action>
        AmbiguitySet<ValueType, State, Action>::AmbiguitySet(
                std::vector<AmbiguityArea<ValueType>> intersections,
                Rectangularity rectangularity,
                std::map<std::pair<State, Action>,
                std::vector<ValueType>> constantTerm,
                std::map<std::pair<State, Action>,
                storm::storage::SparseMatrix<ValueType>> coefficientTerm) :
            intersections(intersections), rectangularity(rectangularity),
            constantTerm(constantTerm), coefficientTerm(coefficientTerm) {

            if (intersections.size() > 0) {
                dimensions = intersections[0].vector.size();
            }
        }

        template class AmbiguitySetBuilder<uint64_t, uint64_t, double>;
        template class AmbiguitySetBuilder<uint64_t, uint64_t, storm::RationalNumber>;
        template class AmbiguitySetBuilder<uint64_t, uint64_t, storm::RationalFunction>;

        template class AmbiguitySet<storm::RationalNumber, uint64_t, uint64_t>;
    }
}
