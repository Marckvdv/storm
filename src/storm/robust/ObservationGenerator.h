#pragma once

#include <cstdint>
#include <tuple>
#include "storm/models/sparse/Model.h"
#include "storm/utility/random.h"

namespace storm {
    namespace robust {
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class ObservationGenerator {
            typedef uint64_t State;
            typedef uint64_t Action;
            typedef ValueType Reward;
            typedef storm::models::sparse::Model<ValueType, RewardModelType> ModelType;
            typedef std::tuple<Action, Reward, State> Transition;

        public:
            ObservationGenerator(ModelType const& model) : model(model) {
            }

            void printTransition(Transition const& t) {
                Action action; Reward reward; State state;
                std::tie(action, reward, state) = t;

                auto choiceLabeling = model.getChoiceLabeling();
                auto labels = choiceLabeling.getLabelsOfChoice(action);

                STORM_PRINT(action);
                if (!labels.empty()) {
                    STORM_PRINT("[");
                    for (auto s : labels) {
                        STORM_PRINT(s << ",");
                    }
                    STORM_PRINT("]");
                }
                STORM_PRINT("-> " << state);

                auto stateLabeling = model.getStateLabeling();
                labels = stateLabeling.getLabelsOfState(state);

                if (!labels.empty()) {
                    STORM_PRINT("{");
                    for (auto s : labels) {
                        STORM_PRINT(s << ",");
                    }
                    STORM_PRINT("}");
                }

                STORM_PRINT(" (" << (reward < storm::utility::zero<ValueType>() ? "-" : "+") << reward << ")" << std::endl);
            }

            Action randomAction() {
                return generator.random_uint(0, model.getTransitionMatrix().getRowGroupSize(currentState) - 1);
            }

            Transition step(Action action) {
                auto row = model.getTransitionMatrix().getRowGroupIndices()[currentState] + action;

                //ValueType probability = generator.random();
                ValueType probability = storm::utility::zero<ValueType>();
                ValueType sum = storm::utility::zero<ValueType>();
                for (auto const& entry : model.getTransitionMatrix().getRow(row)) {
                    sum += entry.getValue();

                    // probability is below sum, we take this transition
                    if (probability < sum) {
                        currentState = entry.getColumn();
                        break;
                    }
                }

                Transition result = std::make_tuple(action, storm::utility::zero<ValueType>(), currentState);
                return result;
            }

            Transition randomStep() {
                return step(randomAction());
            }

            void newTrace(int steps) {
                transitions.clear();

                // TODO pick random initial state
                initialState = *model.getInitialStates().begin();
                currentState = initialState;

                for (int i = 0; i < steps; ++i) {
                    auto newTransition = randomStep();
                    currentState = std::get<2>(newTransition);
                    transitions.push_back(newTransition);
                }
            }

            void generateObservations(int traces, int steps) {
                for (int i = 0; i < traces; ++i) {
                    newTrace(steps);

                    STORM_PRINT("=== Trace " << i << " ===" << std::endl);
                    STORM_PRINT("Initial state: " << initialState << std::endl);
                    for (Transition const& t : transitions) {
                        printTransition(t);
                    }
                }
            }

        private:
            State currentState;
            State initialState;
            std::vector<Transition> transitions;
            ModelType const& model;
            storm::utility::RandomProbabilityGenerator<double> generator;
        };
    }
}
