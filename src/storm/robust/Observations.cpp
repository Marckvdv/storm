#include "storm/robust/Observations.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/JsonAdapter.h"

namespace storm {
    namespace robust {
        // Transition
        template<typename State, typename Action, typename Reward>
        Transition<State, Action, Reward>::Transition(State state, Action action, Reward reward)
            : state(state), action(action), reward(reward) {
        }

        template<typename State, typename Action, typename Reward>
        State& Transition<State, Action, Reward>::getState() {
            return state;
        }

        template<typename State, typename Action, typename Reward>
        State const& Transition<State, Action, Reward>::getState() const {
            return state;
        }

        template<typename State, typename Action, typename Reward>
        Action& Transition<State, Action, Reward>::getAction() {
            return action;
        }

        template<typename State, typename Action, typename Reward>
        Action const& Transition<State, Action, Reward>::getAction() const {
            return action;
        }

        template<typename State, typename Action, typename Reward>
        Reward& Transition<State, Action, Reward>::getReward() {
            return reward;
        }

        template<typename State, typename Action, typename Reward>
        Reward const& Transition<State, Action, Reward>::getReward() const {
            return reward;
        }

        // Trace
        template<typename State, typename Action, typename Reward>
        Trace<State, Action, Reward>::Trace(State initialState)
            : initialState(initialState) {
        }

        template<typename State, typename Action, typename Reward>
        void Trace<State, Action, Reward>::addTransition(Transition<State, Action, Reward> transition) {
            transitions.push_back(transition);
        }

        template<typename State, typename Action, typename Reward>
        State Trace<State, Action, Reward>::getInitialState() const {
            return initialState;
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Transition<State, Action, Reward>>& Trace<State, Action, Reward>::getTransitions() {
            return transitions;
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Transition<State, Action, Reward>> const& Trace<State, Action, Reward>::getTransitions() const {
            return transitions;
        }

        // Observations
        template<typename State, typename Action, typename Reward>
        Observations<State, Action, Reward>::Observations() {
        }

        template<typename State, typename Action, typename Reward>
        Observations<State, Action, Reward>::Observations(std::vector<Trace<State, Action, Reward>> traces) : traces(traces) {
        }

        template<typename State, typename Action, typename Reward>
        void Observations<State, Action, Reward>::addTrace(Trace<State, Action, Reward> trace) {
            traces.push_back(trace);
        }

        // Write observations to an output stream
        template<typename State, typename Action, typename Reward>
        void Observations<State, Action, Reward>::writeToFile(std::ostream& output) const {
            using json = storm::json<double>;

            // Transform the observations into an JSON array
            auto result = json::array();
            for (auto const& trace : traces) {
                json traceJson;
                traceJson["initial"] = std::to_string(trace.getInitialState());

                auto transitionsJson = json::array();
                for (auto const& transition : trace.getTransitions()) {
                    auto transitionJson = json::array();

                    // Append transition components to the `transitionJson`
                    // array.
                    transitionJson += std::to_string(transition.getAction());
                    //transitionJson += std::to_string(transition.getReward());
                    transitionJson += 0; // TODO rewards
                    transitionJson += std::to_string(transition.getState());

                    transitionsJson += transitionJson;
                }
                traceJson["transitions"] = transitionsJson;
                result += traceJson;
            }

            // Write the resulting JSON as a string to the output stream
            output << result << std::endl;
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Trace<State, Action, Reward>>& Observations<State, Action, Reward>::getTraces() {
            return traces;
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Trace<State, Action, Reward>> const& Observations<State, Action, Reward>::getTraces() const {
            return traces;
        }

        template class Trace<uint64_t, uint64_t, double>;
        template class Trace<uint64_t, uint64_t, storm::RationalFunction>;
        template class Trace<uint64_t, uint64_t, storm::RationalNumber>;

        template class Transition<uint64_t, uint64_t, double>;
        template class Transition<uint64_t, uint64_t, storm::RationalFunction>;
        template class Transition<uint64_t, uint64_t, storm::RationalNumber>;

        template class Observations<uint64_t, uint64_t, double>;
        template class Observations<uint64_t, uint64_t, storm::RationalFunction>;
        template class Observations<uint64_t, uint64_t, storm::RationalNumber>;
    }
}
