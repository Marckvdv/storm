#include "JaniRegionFromObservationsBuilder.h"

#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/ModelType.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/storage/jani/Assignment.h"
#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"

#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parametric.h"

#include <string>
#include <unordered_map>
#include <boost/math/special_functions/beta.hpp>

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename Reward>
        JaniRegionFromObservationsBuilder<State, Action, Reward>::JaniRegionFromObservationsBuilder(Observations<State, Action, Reward> observations) :
            observations(observations) {
        }

#define TransitionsMap std::map<State, std::map<Action, std::map<State, uint64_t>>>
        template<typename State, typename Action, typename Reward>
        TransitionsMap JaniRegionFromObservationsBuilder<State, Action, Reward>::calculateTransitionsMap(State& highestState, Action& highestAction) {
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


        template <typename State, typename Action, typename Reward>
        std::pair<storm::jani::Model, storm::storage::ParameterRegion<storm::RationalFunction>> JaniRegionFromObservationsBuilder<State, Action, Reward>::build() {
            /*
            storm::utility::parametric::Valuation<storm::RationalFunction> lowerBound;
            storm::utility::parametric::Valuation<storm::RationalFunction> upperBound;
            uint64_t variableCount = 0;

            storm::jani::Model model("mainModel", storm::jani::ModelType::MDP);
            auto& exp = model.getExpressionManager();
            storm::jani::Automaton automaton("mainAutomaton", exp.declareIntegerVariable("pc"));

            State highestState = 0;
            Action highestAction = 0;
            auto transitions = calculateTransitionsMap(highestState, highestAction);

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

            // First add all actions and locations.
            for (auto const& t1 : transitions) {
                auto &s1 = t1.first;
                addState(s1);

                auto& v1 = t1.second;
                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    addAction(action);

                    auto &v2 = t2.second;
                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        addState(s2);
                    }
                }
            }

            // Next, add all edges
            //
            // Loop over begin states
            for (auto const& t1 : transitions) {
                auto &s1 = t1.first;
                auto& v1 = t1.second;
                uint64_t beginLoc = automaton.getLocationIndex(states.at(s1).getName());

                // Loop over actions
                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;

                    uint64_t total = 0;
                    // Loop over end states
                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        uint64_t amount = t3.second;
                        total += amount;
                    }

                    uint64_t startingVariable = variableCount;
                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationsAndProbabilities;
                    size_t uniqueStateCount = v2.size();
                    size_t currentState = 0;
                    // Loop over end states
                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        uint64_t amount = t3.second;
                        uint64_t endLoc = automaton.getLocationIndex(states.at(s2).getName());

                        // Check if it's the last state
                        if (currentState+1 != uniqueStateCount) {
                            double lb;
                            double ub;
                            std::tie(lowerBound, upperBound) = calculateLowerUpperBound(amount, total, 0.95);

                            storm::RationalFunctionVariable newVariable(variableCount++);
                            storm::RationalFunction fun(newVariable);
                            auto expr = storm::expressions::RationalFunctionToExpression<storm::RationalFunction>(exp).toExpression(fun);
//                            auto prob = exp.rational(storm::RationalNumber(amount) / total);
                            destinationsAndProbabilities.push_back({endLoc, expr});
                            lowerBound[newVariable] = storm::RationalFunction(storm::RationalNumber(lb));
                            upperBound[newVariable] = storm::RationalFunction(storm::RationalNumber(ub));
                        } else {
                        }

                        currentState++;
                    }

                    uint64_t actionIndex = model.getActionIndex(actions.at(action).getName());
                    auto templateEdge = std::make_shared<storm::jani::TemplateEdge>(exp.boolean(true));
                    // Loop over end states
                    for (size_t i = 0; i < v2.size(); ++i) {
                        templateEdge->addDestination(storm::jani::TemplateEdgeDestination());
                    }
                    automaton.registerTemplateEdge(templateEdge);

                    storm::jani::Edge edge(beginLoc, actionIndex, boost::none, templateEdge, destinationsAndProbabilities);
                    automaton.addEdge(edge);
                }

            }

            for (auto const& trace : observations.getTraces()) {
                auto initial = trace.getInitialState();
                automaton.addInitialLocation(states.at(initial).getName());
            }

            model.addAutomaton(automaton);
            model.setStandardSystemComposition();
            model.finalize();

            storm::storage::ParameterRegion<storm::RationalFunction> region(lowerBound, upperBound);
            return std::make_pair(model, region);
            */
        }

        template <typename State, typename Action, typename Reward>
        std::pair<double, double> JaniRegionFromObservationsBuilder<State, Action, Reward>::calculateLowerUpperBound(uint64_t part, uint64_t total, double confidence) {
            double lower = boost::math::ibeta_inv(part, total-part,   confidence/2);
            double upper = boost::math::ibeta_inv(part, total-part, 1-confidence/2);

            return std::make_pair(lower, upper);
        }


        template class JaniRegionFromObservationsBuilder<uint64_t, uint64_t, double>;
    }
}
