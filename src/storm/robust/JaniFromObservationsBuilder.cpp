#include "JaniFromObservationsBuilder.h"

#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/ModelType.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/Assignment.h"
#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"

#include <string>
#include <unordered_map>

namespace storm {
    namespace robust {
        template <typename State, typename Action, typename Reward>
        JaniFromObservationsBuilder<State, Action, Reward>::JaniFromObservationsBuilder(Observations<State, Action, Reward> observations) :
            observations(observations) {
        }

#define TransitionsMap std::map<State, std::map<Action, std::map<State, uint64_t>>>
        template<typename State, typename Action, typename Reward>
        TransitionsMap JaniFromObservationsBuilder<State, Action, Reward>::calculateTransitionsMap(State& highestState, Action& highestAction) {
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
        storm::jani::Model JaniFromObservationsBuilder<State, Action, Reward>::build() {
            const auto name = std::string("mainModel");
            const auto mt = storm::jani::ModelType::MDP;

            storm::jani::Model model(name, mt);
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
            for (auto const& t1 : transitions) {
                auto &s1 = t1.first;
                auto& v1 = t1.second;
                uint64_t beginLoc = automaton.getLocationIndex(states.at(s1).getName());

                for (auto const& t2 : v1) {
                    auto action = t2.first;
                    auto &v2 = t2.second;

                    uint64_t total = 0;
                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        uint64_t amount = t3.second;
                        total += amount;
                    }

                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationsAndProbabilities;
                    for (auto const& t3 : v2) {
                        auto s2 = t3.first;
                        uint64_t amount = t3.second;

                        uint64_t endLoc = automaton.getLocationIndex(states.at(s2).getName());
                        auto prob = exp.rational(storm::RationalNumber(amount) / total);
                        destinationsAndProbabilities.push_back({endLoc, prob});
                    }

                    uint64_t actionIndex = model.getActionIndex(actions.at(action).getName());
                    auto templateEdge = std::make_shared<storm::jani::TemplateEdge>(exp.boolean(true));
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
            return model;
        }

        template class JaniFromObservationsBuilder<uint64_t, uint64_t, double>;
    }
}
