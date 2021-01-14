#pragma once

#include <vector>
#include <cstring>
#include <istream>
#include <ostream>

namespace storm {
    namespace robust {
        // A single transition
        template<typename State, typename Action, typename Reward>
        class Transition {
            // The next state
            State state;

            // The action
            Action action;

            // The reward
            Reward reward;

        public:
            Transition(State state, Action action, Reward reward);

            // Getters and their const versions.
            State& getState();
            State const& getState() const;

            Action& getAction();
            Action const& getAction() const;

            Reward& getReward();
            Reward const& getReward() const;
        };

        // A single trace i.e. a sequence of Transition's
        template<typename State, typename Action, typename Reward>
        class Trace {
            State initialState;
            std::vector<Transition<State, Action, Reward>> transitions;

        public:
            Trace(State initialState, int capacity = 0);
            void addTransition(Transition<State, Action, Reward> transition);
            State getInitialState() const;

            std::vector<Transition<State, Action, Reward>>& getTransitions();
            std::vector<Transition<State, Action, Reward>> const& getTransitions() const;
        };

        // Observations, a collection of Trace's
        template<typename State, typename Action, typename Reward>
        class Observations {
            std::vector<Trace<State, Action, Reward>> traces;
            typedef Trace<State, Action, Reward> TraceType;

        public:
            Observations(int capacity = 0);
            Observations(std::vector<TraceType> traces);
            void addTrace(TraceType trace);

            // Write the observations to the output stream
            void writeToFile(std::ostream& output) const;

            std::vector<TraceType>& getTraces();
            std::vector<TraceType> const& getTraces() const;
        };
    }
}
