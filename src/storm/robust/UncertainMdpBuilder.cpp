#include "UncertainMdpBuilder.h"

#include "storm/utility/constants.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/ModelType.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/Assignment.h"
#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace robust {
        template <typename ValueType>
        TransitionInformation<ValueType>::TransitionInformation() :
            transitionInterval(std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())),
            sampleInterval(std::make_pair(storm::utility::one<ValueType>(), storm::utility::one<ValueType>())) {
        }

        template <typename ValueType>
        TransitionInformation<ValueType>::TransitionInformation(std::pair<ValueType, ValueType> transitionInterval, std::pair<ValueType, ValueType> sampleInterval) :
            transitionInterval(transitionInterval),
            sampleInterval(sampleInterval) {
        }

        template <typename ValueType>
        void TransitionInformation<ValueType>::updateInformation(ValueType part, ValueType total) {
            auto ratio = part / total;
            // Workaround for RationalFunction only having == and <
            //if (ratio >= transitionInterval.first) {
            if (transitionInterval.first < ratio || transitionInterval.first == ratio) {
                transitionInterval.first = (sampleInterval.second*transitionInterval.first + part)/(sampleInterval.second + total);
            } else {
                transitionInterval.first = (sampleInterval.first*transitionInterval.first + part)/(sampleInterval.first + total);
            }

            // Workaround for RationalFunction only having == and <
            //if (ratio <= transitionInterval.second) {
            if (ratio < transitionInterval.second || ratio == transitionInterval.second) {
                transitionInterval.second = (sampleInterval.second*transitionInterval.second + part)/(sampleInterval.second + total);
            } else {
                transitionInterval.second = (sampleInterval.first*transitionInterval.second + part)/(sampleInterval.first + total);
            }

            sampleInterval.first += total;
            sampleInterval.second += total;
        }

#define TransitionsMap std::map<State, std::map<Action, std::map<State, uint64_t>>>
        template <typename State, typename Action, typename Reward, typename ValueType>
        UncertainMdpBuilder<State, Action, Reward, ValueType>::UncertainMdpBuilder() {
        }

        template <typename State, typename Action, typename Reward, typename ValueType>
        void UncertainMdpBuilder<State, Action, Reward, ValueType>::processObservations(Observations<State, Action, Reward> const& observations) {
            auto transitionsMap = calculateTransitionsMap(observations);
            for (auto const& t1 : transitionsMap) {
                auto& s1 = t1.first;
                auto& v1 = t1.second;

                // Loop over actions
                for (auto const& t2 : v1) {
                    auto &action = t2.first;
                    auto &v2 = t2.second;

                    uint64_t total = 0;
                    for (auto const& t3 : v2) {
                        auto& s2 = t3.first;
                        uint64_t amount = t3.second;
                        total += amount;
                    }

                    for (auto const& t3 : v2) {
                        auto &s2 = t3.first;
                        uint64_t amount = t3.second;
                        auto t = std::make_tuple(s1, action, s2);

                        auto it = info.find(t);
                        if (it == info.end()) {
                            info[t] = TransitionInformation<ValueType>();
                        }
                        auto& i = info[t];
                        i.updateInformation(ValueType(amount), ValueType(total));
                    }
                }
            }

            for (auto const &kv : info) {
                auto const &k = kv.first;
                auto const &v = kv.second;

                State s1; Action a; State s2;
                std::tie(s1, a, s2) = k;


                ValueType lowerT, upperT, lowerS, upperS;
                std::tie(lowerT, upperT) = v.transitionInterval;
                std::tie(lowerS, upperS) = v.sampleInterval;
                std::cout << "(" << s1 << " " << a << " -> " << s2 << ") =>[" << lowerT << "," << upperT << "] (" << lowerS << "," << upperS << ")" << std::endl;
            }
        }

        template <typename State, typename Action, typename Reward, typename ValueType>
        storm::jani::Model UncertainMdpBuilder<State, Action, Reward, ValueType>::buildMdp() {
//            storm::utility::parametric::Valuation<storm::RationalFunction> lowerBound, upperBound;
            std::unordered_map<storm::expressions::Variable, ValueType> lowerBound, upperBound;
            
            uint64_t variableCount = 0;

            storm::jani::Model model("mainModel", storm::jani::ModelType::MDP);
            auto& exp = model.getExpressionManager();
            storm::jani::Automaton automaton("mainAutomaton", exp.declareIntegerVariable("pc"));

            std::unordered_map<State, storm::jani::Location> states;
            std::unordered_map<Action, storm::jani::Action> actions;

            auto addState = [&automaton, &states] (auto state) {
                auto search = states.find(state);
                if (search == states.end()) {
                    storm::jani::Location loc(std::to_string(state));
                    states.insert({state, loc});
                    automaton.addLocation(loc);
                }
            };

            auto addAction = [&model, &actions] (auto action) {
                auto search = actions.find(action);
                if (search == actions.end()) {
                    storm::jani::Action newAction(std::to_string(action));
                    actions.insert({action, newAction});
                    model.addAction(newAction);
                }
            };

            std::map<std::pair<State, Action>, std::vector<std::pair<uint64_t, storm::expressions::Expression>>> probs;

            for (auto const& k : info) {
                State s1; Action a; State s2;
                std::tie(s1, a, s2) = k.first;

                addState(s1);
                addAction(a);
                addState(s2);

                ValueType lb, ub;
                std::tie(lb, ub) = k.second.transitionInterval;

                auto p = std::make_pair(s1, a);
                if (probs.find(p) == probs.end()) {
                    probs[p] = std::vector<std::pair<uint64_t, storm::expressions::Expression>>();
                }

                auto var = exp.declareFreshRationalVariable();
                lowerBound[var] = lb;
                upperBound[var] = ub;

                auto expr = exp.getVariableExpression(var.getName());
                uint64_t endLoc = automaton.getLocationIndex(states.at(s1).getName());
                probs[p].push_back({endLoc, expr});
            }

            for (auto const& kv : probs) {
                auto const& k = kv.first;
                auto const& v = kv.second;

                State s1; Action a;
                std::tie(s1, a) = k;

                uint64_t beginLoc = automaton.getLocationIndex(states.at(s1).getName());
                uint64_t actionIndex = model.getActionIndex(actions.at(a).getName());
                auto templateEdge = std::make_shared<storm::jani::TemplateEdge>(exp.boolean(true));
                for (size_t i = 0; i < v.size(); ++i) {
                    templateEdge->addDestination(storm::jani::TemplateEdgeDestination());
                }
                storm::jani::Edge edge(beginLoc, actionIndex, boost::none, templateEdge, v);
                automaton.addEdge(edge);
            }

            model.addAutomaton(automaton);
            model.setStandardSystemComposition();
            model.finalize();

            //storm::storage::ParameterRegion<storm::RationalFunction> region(lowerBound, upperBound);
            //return std::make_pair(model, region);
            return model;
        }

        template<typename State, typename Action, typename Reward, typename ValueType>
        TransitionsMap UncertainMdpBuilder<State, Action, Reward, ValueType>::calculateTransitionsMap(Observations<State, Action, Reward> const& observations) {
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

            for (const auto &trace : observations.getTraces()) {
                State currentState = trace.getInitialState();

                for (const auto &transition : trace.getTransitions()) {
                    Action action = transition.getAction();
                    State nextState = transition.getState();

                    addTransition(currentState, action, nextState, transitions);

                    auto max = [](auto x, auto y) { return (x > y ? x : y); };
                    currentState = nextState;
                }
            }

            return transitions;
        }

        template class UncertainMdpBuilder<uint64_t, uint64_t, double, double>;
        template class UncertainMdpBuilder<uint64_t, uint64_t, storm::RationalNumber, storm::RationalNumber>;
        template class UncertainMdpBuilder<uint64_t, uint64_t, storm::RationalFunction, storm::RationalFunction>;
    }
}
