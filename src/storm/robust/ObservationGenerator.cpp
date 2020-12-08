#include "storm/robust/ObservationGenerator.h"

#define TEMPLATE_DEF template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>

namespace storm {
    namespace robust {
        TEMPLATE_DEF
        ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::ObservationGenerator(ModelType const& model, Policy<State, Action, ValueType>& policy) :
            model(model), observations(Observations<State, Action, Reward>()),
            policy(policy) {
        }

        TEMPLATE_DEF
        Action ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::randomAction() {
            return generator.random_uint(0, model.getTransitionMatrix().getRowGroupSize(currentState) - 1);
        }

        TEMPLATE_DEF
        Transition<State, Action, Reward> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::step(Action action) {
            auto row = model.getTransitionMatrix().getRowGroupIndices()[currentState] + action;

            ValueType probability = generator.random();
            //ValueType probability = storm::utility::zero<ValueType>();
            ValueType sum = storm::utility::zero<ValueType>();
            auto transitions = model.getTransitionMatrix();
            auto matrixRow = transitions.getRow(row);
            for (auto const& entry : matrixRow) {
                sum += entry.getValue();

                // probability is below sum, we take this transition
                if (probability < sum) {
                    currentState = entry.getColumn();
                    break;
                }
            }

            Transition<State, Action, Reward> result(currentState, action, storm::utility::zero<ValueType>());
            return result;
        }

        TEMPLATE_DEF
        Transition<State, Action, Reward> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::randomStep() {
            auto action = policy.getNextAction(generator, currentState);
            return step(action);
        }

        TEMPLATE_DEF
        void ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::newTrace(int steps) {
            // TODO pick random initial state
            auto initialState = *model.getInitialStates().begin();
            Trace<State, Action, Reward> trace(initialState);
            currentState = initialState;

            policy.addHistory(currentState);

            for (int i = 0; i < steps; ++i) {
                auto newTransition = randomStep();
                currentState = newTransition.getState();
                policy.addHistory(currentState);
                trace.addTransition(newTransition);
            }

            observations.addTrace(trace);
        }

        TEMPLATE_DEF
        Observations<State, Action, Reward> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::generateObservations(int traces, int steps) {
            for (int i = 0; i < traces; ++i) {
                newTrace(steps);
            }

            return observations;
        }

        template class ObservationGenerator<uint64_t, uint64_t, double, double>;
        template class ObservationGenerator<uint64_t, uint64_t, storm::RationalFunction, storm::RationalFunction>;
        template class ObservationGenerator<uint64_t, uint64_t, storm::RationalNumber, storm::RationalNumber>;
    }
}

#undef TEMPLATE_DEF
