#include "storm/robust/Observations.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

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
        Action& Transition<State, Action, Reward>::getAction() {
            return action;
        }

        template<typename State, typename Action, typename Reward>
        Reward& Transition<State, Action, Reward>::getReward() {
            return reward;
        }

        template<typename State, typename Action, typename Reward>
        void Transition<State, Action, Reward>::writeToFile(std::ostream& output) const {
                output
                    << "[" << action << "] --> " << state << " (" << reward
                    << ")" << std::endl;
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
        void Trace<State, Action, Reward>::writeToFile(std::ostream& output) const {
            output << "Initial state: " << initialState << std::endl;
            for (auto const& t : transitions) {
                t.writeToFile(output);
            }
        }

        template<typename State, typename Action, typename Reward>
        State Trace<State, Action, Reward>::getInitialState() {
            return initialState;
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Transition<State, Action, Reward>>& Trace<State, Action, Reward>::getTransitions() {
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

        template<typename State, typename Action, typename Reward>
        void Observations<State, Action, Reward>::writeToFile(std::ostream& output) const {
            for (int i = 0; i < traces.size(); ++i) {
                output << "Trace " << i << std::endl;
                traces[i].writeToFile(output);
            }
        }

        template<typename State, typename Action, typename Reward>
        Observations<State, Action, Reward> Observations<State, Action, Reward>::readFromFile(std::istream& input) {
            if (!readExact(input, "Trace"))
                throw 1;

            std::vector<Trace<State, Action, Reward>> traces;
            while (input.good()) {
                //auto trace = Trace<State, Action, Reward>::readFromFile(input);
                //traces.push_back(trace);
            }

            return Observations(traces);
        }

        template<typename State, typename Action, typename Reward>
        std::vector<Trace<State, Action, Reward>>& Observations<State, Action, Reward>::getTraces() {
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
