#include "storm/robust/ObservationSparseModelBuilder.h"

namespace storm {
    namespace robust {
        typedef storm::storage::SparseMatrix<storm::RationalNumber> TransitionMatrix;
        typedef storm::models::sparse::Mdp<storm::RationalNumber> Mdp;
#define TransitionsMap std::map<State, std::map<Action, std::map<State, uint64_t>>>

        template<typename State, typename Action, typename Reward>
        TransitionsMap ObservationSparseModelBuilder<State, Action, Reward>::calculateTransitionsMap(State& highestState, Action& highestAction) {
            TransitionsMap transitions;
            auto addTransition = [](State s1, Action action, State s2, auto& map) {
                auto it = map.find(s1);
                if (it == map.end()) {
                    map[s1] = {{ action, {{ s2, 1 }} }};
                    return;
                }

                auto& map2 = (*it).second;
                auto it2 = map2.find(action);
                if (it2 == map2.end()) {
                    map2[action] = {{ s2, 1 }};
                    return;
                }

                auto& map3 = (*it2).second;
                auto it3 = map3.find(s2);
                if (it3 == map3.end()) {
                    map3[s2] = 1;
                    return;
                }

                map3[s2] += 1;
            };

            for (auto &trace : observations.getTraces()) {
                State currentState = trace.getInitialState();

                for (auto &transition : trace.getTransitions()) {
                    Action action = transition.getAction();
                    State nextState = transition.getState();

                    addTransition(currentState, action, nextState, transitions);

                    auto max = [](auto x, auto y) { return (x > y ? x : y); };
                    highestState = max(max(highestState, currentState), nextState);
                    highestAction = max(highestAction, action);
                    currentState = nextState;

                }
            }

            return transitions;
        }

        template<typename State, typename Action, typename Reward>
        ObservationSparseModelBuilder<State, Action, Reward>::ObservationSparseModelBuilder(ObservationsType observations) :
            observations(observations) {}

        template<typename State, typename Action, typename Reward>
        Mdp ObservationSparseModelBuilder<State, Action, Reward>::buildMdp() {
            Action highestAction;
            State highestState;
            TransitionsMap transitions = calculateTransitionsMap(highestState, highestAction);

            Action numActions = highestAction + 1;
            State numStates = highestState + 1;

            printTransitions(transitions);

            storm::storage::SparseMatrixBuilder<storm::RationalNumber> builder(numStates*numActions, numStates, 0, false, true, numStates);

            State lastState = 0;
            for (auto const& t1 : transitions) {
                auto s1 = t1.first;
                auto& v1 = t1.second;

                std::cout << "S: " << s1 << std::endl;
                for (State s = lastState; s <= s1; ++s) {
                    std::cout << "s: " << s << std::endl;
                    builder.newRowGroup(s * numActions);
                }

                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;

                    uint64_t total = 0;
                    for (auto const& t3 : v2) {
                        auto amount = t3.second;
                        total += amount;
                    }

                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        auto amount = t3.second;

                        builder.addNextValue(s1*numActions + action, s2, storm::RationalNumber(amount, total));
                    }

                    lastState = s1+1;
                }
            }

            auto transitionMatrix = builder.build();
            printSparseMatrix(transitionMatrix);

            Mdp mdp(transitionMatrix, storm::models::sparse::StateLabeling(numStates));

            return mdp;
        }

        template<typename State, typename Action, typename Reward>
        void ObservationSparseModelBuilder<State, Action, Reward>::printTransitions(TransitionsMap const& transitions) {
            std::cout << "T:" << transitions.size() << std::endl;
            for (auto const& t1 : transitions) {
                auto& k1 = t1.first;
                auto& v1 = t1.second;
                for (auto const& t2 : v1) {
                    auto &k2 = t2.first;
                    auto &v2 = t2.second;
                    for (auto const& t3 : v2) {
                        auto &k3 = t3.first;
                        auto &v3 = t3.second;
                        std::cout << "transitions[" << k1 << "][" << k2 << "][" << k3 << "] = " << v3 << std::endl;
                    }
                }
            }
        }


        template class ObservationSparseModelBuilder<uint64_t, uint64_t, double>;
        template class ObservationSparseModelBuilder<uint64_t, uint64_t, storm::RationalNumber>;
        template class ObservationSparseModelBuilder<uint64_t, uint64_t, storm::RationalFunction>;
    }
}
