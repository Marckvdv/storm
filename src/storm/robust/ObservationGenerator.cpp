#include "storm/robust/ObservationGenerator.h"

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::ObservationGenerator(ModelType const& model, Policy<State, Action, ValueType>& policy) :
            model(model), observations(Observations<State, Action, Reward>()),
            policy(policy) {
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        State ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::randomInitialState() {
            // Collect all initial states
            auto& initialStates = model.getInitialStates();
            std::vector<State> states;
            for (const auto s : initialStates) {
                states.push_back(s);
            }

            // Pick a random one
            auto randomIndex = generator.random_uint(0, states.size()-1);
            return states[randomIndex];
        }

        // Perform a single step
        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        boost::optional<Transition<State, Action, Reward>> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::step(Action action) {
            // Get the row for the current state
            auto row = model.getTransitionMatrix().getRowGroupIndices()[currentState] + action;

            // Determine what transition to take
            ValueType probability = generator.random();
            ValueType sum = storm::utility::zero<ValueType>();
            auto transitions = model.getTransitionMatrix();
            auto matrixRow = transitions.getRow(row);
            boost::optional<Transition<State, Action, Reward>> result;
            for (auto const& entry : matrixRow) {
                sum += entry.getValue();

                // probability is below sum, we take this transition
                if (probability < sum) {
                    currentState = entry.getColumn();
                    // TODO non-zero reward
                    result = Transition<State, Action, Reward>(currentState, action, storm::utility::zero<ValueType>());
                    break;
                }
            }

            return result;
        }

        // Perform a random step based on the chosen action of the policy
        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        boost::optional<Transition<State, Action, Reward>> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::randomStep() {
            auto action = policy.getNextAction(generator, currentState);
            return step(action);
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        void ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::newTrace(int steps, bool useRandomInitialState, State initialState) {
            // Determine initial state
            if (useRandomInitialState) {
                initialState = randomInitialState();
            }
            Trace<State, Action, Reward> trace(initialState);
            currentState = initialState;

            // Add the initial state to the history of states visited
            policy.addHistory(currentState);

            // Loop over all steps and add to the observerd transitions
            for (int i = 0; i < steps; ++i) {
                if (auto newTransition = randomStep()) {
                    auto unwrapped = *newTransition;
                    currentState = unwrapped.getState();
                    policy.addHistory(currentState);
                    trace.addTransition(unwrapped);
                } else {
                    std::cout << "deadlock!" << std::endl;
                    break;
                }
            }

            observations.addTrace(trace);
        }

        template<typename State, typename Action, typename Reward, typename ValueType, typename RewardModelType>
        Observations<State, Action, Reward> ObservationGenerator<State, Action, Reward, ValueType, RewardModelType>::generateObservations(int traces, int steps, bool useRandomInitialState, State initialState) {
            for (int i = 0; i < traces; ++i) {
                newTrace(steps, useRandomInitialState, initialState);
            }

            return observations;
        }

        template class ObservationGenerator<uint64_t, uint64_t, double, double>;
        template class ObservationGenerator<uint64_t, uint64_t, storm::RationalFunction, storm::RationalFunction>;
        template class ObservationGenerator<uint64_t, uint64_t, storm::RationalNumber, storm::RationalNumber>;
    }
}

#undef TEMPLATE_DEF
