#pragma once

#include <vector>
#include <cstring>
#include <istream>
#include <ostream>

namespace storm {
    namespace robust {
        template<typename State, typename Action, typename Reward>
        class Transition {
            State state;
            Action action;
            Reward reward;

        public:
            Transition(State state, Action action, Reward reward);
            State& getState();
            State const& getState() const;
            Action& getAction();
            Action const& getAction() const;
            Reward& getReward();
            Reward const& getReward() const;
            void writeToFile(std::ostream& output) const;
        };

        template<typename State, typename Action, typename Reward>
        class Trace {
            State initialState;
            std::vector<Transition<State, Action, Reward>> transitions;

        public:
            Trace(State initialState);
            void addTransition(Transition<State, Action, Reward> transition);
            void writeToFile(std::ostream& output) const;
            State getInitialState() const;
            std::vector<Transition<State, Action, Reward>>& getTransitions();
            std::vector<Transition<State, Action, Reward>> const& getTransitions() const;
        };

        template<typename State, typename Action, typename Reward>
        class Observations {
            std::vector<Trace<State, Action, Reward>> traces;
            typedef Trace<State, Action, Reward> TraceType;

        public:
            Observations();
            Observations(std::vector<TraceType> traces);
            void addTrace(TraceType trace);
            void writeToFile(std::ostream& output) const;
            std::vector<TraceType>& getTraces();
            std::vector<TraceType> const& getTraces() const;
        };
    }
}
