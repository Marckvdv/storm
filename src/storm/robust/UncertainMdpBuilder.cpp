#include "UncertainMdpBuilder.h"

#include "storm/utility/constants.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/ModelType.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/Assignment.h"
#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/adapters/JsonAdapter.h"
#include "storm/robust/Utility.h"
#include "storm/io/file.h"

namespace storm {
    namespace robust {
        template <typename ValueType>
        TransitionInformation<ValueType>::TransitionInformation() : TransitionInformation(storm::utility::one<ValueType>()) {
        }

        template <typename ValueType>
        TransitionInformation<ValueType>::TransitionInformation(ValueType initialPrior) :
            transitionInterval(std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())),
            sampleInterval(std::make_pair(initialPrior, initialPrior)),
            branchTaken(std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {
        }

        template <typename ValueType>
        TransitionInformation<ValueType>::TransitionInformation(std::pair<ValueType, ValueType> transitionInterval, std::pair<ValueType, ValueType> sampleInterval) :
            transitionInterval(transitionInterval),
            sampleInterval(sampleInterval),
            branchTaken(std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {

        }

        template <typename ValueType>
        void TransitionInformation<ValueType>::updateInformation(ValueType part, ValueType total) {
            auto ratio = part / total;
            // Workaround for RationalFunction only having == and <
            //if (ratio >= transitionInterval.first) {
            if (transitionInterval.first < ratio || transitionInterval.first == ratio) {
                transitionInterval.first = (sampleInterval.second*transitionInterval.first + part)/(sampleInterval.second + total);
                branchTaken.first += 1;
            } else {
                transitionInterval.first = (sampleInterval.first*transitionInterval.first + part)/(sampleInterval.first + total);
                branchTaken.second += 1;
            }

            // Workaround for RationalFunction only having == and <
            //if (ratio <= transitionInterval.second) {
            if (ratio < transitionInterval.second || ratio == transitionInterval.second) {
                transitionInterval.second = (sampleInterval.second*transitionInterval.second + part)/(sampleInterval.second + total);
                branchTaken.first += 1;
            } else {
                transitionInterval.second = (sampleInterval.first*transitionInterval.second + part)/(sampleInterval.first + total);
                branchTaken.second += 1;
            }

            sampleInterval.first += total;
            sampleInterval.second += total;
        }

#define TransitionsMap std::map<State, std::map<Action, std::map<State, uint64_t>>>
#define PriorType std::map<std::tuple<State, Action, State>, TransitionInformation<ValueType>>
        template <typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::UncertainMdpBuilder(ValueType initialPrior) :
            initialPrior(initialPrior) {
        }

        template <typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        void UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::processObservations(Observations<State, Action, Reward> const& observations) {
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
                            info[t] = TransitionInformation<ValueType>(initialPrior);
                        }
                        auto& i = info[t];
                        i.updateInformation(ValueType(amount), ValueType(total));
                    }
                }
            }

            std::cout << "INFO: " << info.size() << std::endl;
        }

        template <typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        storm::jani::Model UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::buildMdp() {
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

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        TransitionsMap UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::calculateTransitionsMap(Observations<State, Action, Reward> const& observations) {
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

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        void UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::loadPrior(std::string path) {
            std::ifstream file;
            storm::utility::openFile(path, file);
            storm::json<double> structure;
            structure << file;

            for (const auto& entry : structure) {
                State s1 = entry[0];
                Action a = entry[1];
                State s2 = entry[2];

                double lowerTd = entry[3], upperTd = entry[4];
                double lowerSd = entry[5], upperSd = entry[6];
                ValueType lowerT = ValueType(lowerTd), upperT = ValueType(upperTd);
                ValueType lowerS = ValueType(lowerSd), upperS = ValueType(upperSd);
                info[std::make_tuple(s1, a, s2)] = TransitionInformation<ValueType>(std::make_pair(lowerT, upperT), std::make_pair(lowerS, upperS));
            }

            storm::utility::closeFile(file);
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        void UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::loadPrior(PriorType prior) {
            info = prior;
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        void UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::exportIntervals(std::ostream& output) {
            storm::json<double> structure = storm::json<double>::array();
            for (auto const &kv : info) {
                auto const &k = kv.first;
                auto const &v = kv.second;

                State s1; Action a; State s2;
                std::tie(s1, a, s2) = k;

                ValueType lowerT, upperT, lowerS, upperS;
                std::tie(lowerT, upperT) = v.transitionInterval;
                std::tie(lowerS, upperS) = v.sampleInterval;
                storm::json<double> transition = {s1, a, s2, toDouble(lowerT), toDouble(upperT), toDouble(lowerS), toDouble(upperS)};
                structure.push_back(transition);
            }
            output << structure << std::endl;
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        ValueType UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::compareToTrue(storm::models::sparse::Model<ValueType, RewardModelType> const& model) {
            auto transitions = model.getTransitionMatrix();
            auto stateCount = transitions.getRowGroupCount();
            ValueType totalError = storm::utility::zero<ValueType>();

            auto n = 0;
            for (auto state = 0; state < stateCount; ++state) {
                auto choiceCount = transitions.getRowGroupSize(state);
                for (auto action = 0; action < choiceCount; ++action) {
                    auto rowIndex = transitions.getRowGroupIndices()[state] + action;
                    auto row = transitions.getRow(rowIndex);

                    for (auto &entry : row) {
                        n += 1;
                        auto error = storm::utility::zero<ValueType>();
                        auto destination = entry.getColumn();
                        auto triplet = std::make_tuple(state, action, destination);
                        auto interval = std::make_pair(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());
                        auto trueProb = entry.getValue();
                        auto find = info.find(triplet);
                        if (find != info.end()) {
                            auto prior = find->second;
                            interval = prior.transitionInterval;
                        }
                        // Not finding the triplet assumes single value
                        // interval [0, 0]

                        error += storm::utility::abs<ValueType>(trueProb - interval.first);
                        error += storm::utility::abs<ValueType>(trueProb - interval.second);
                        totalError += error / 2;
                    }
                }
            }

            return totalError / n;
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        std::pair<ValueType, ValueType> UncertainMdpBuilder<State, Action, Reward, ValueType, RewardModelType>::branchStatistics() {
            auto total = std::pair<ValueType, ValueType>(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());

            for (auto &prior : info) {
                total.first += prior.second.branchTaken.first;
                total.second += prior.second.branchTaken.second;
            }

            return total;
        }

        template class UncertainMdpBuilder<uint64_t, uint64_t, double, double>;
        template class UncertainMdpBuilder<uint64_t, uint64_t, storm::RationalNumber, storm::RationalNumber>;
        template class UncertainMdpBuilder<uint64_t, uint64_t, storm::RationalFunction, storm::RationalFunction>;
    }
}

#undef TransitionsMap
#undef PriorType