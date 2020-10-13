#pragma once

#include <vector>
#include <cstring>
#include <istream>
#include <ostream>

namespace storm {
    namespace robust {
        static bool readExact(std::istream& input, std::string toRead) {
            // TODO unicode
            std::vector<char> buffer(toRead.size());
            input.read(buffer.data(), toRead.size());

            return std::memcmp(toRead.c_str(), buffer.data(), toRead.size()) == 0;
        }

        template<typename State, typename Action, typename Reward>
        class Transition {
            State state;
            Action action;
            Reward reward;

        public:
            Transition(State state, Action action, Reward reward);
            State& getState();
            Action& getAction();
            Reward& getReward();
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
            State getInitialState();
            std::vector<Transition<State, Action, Reward>>& getTransitions();
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
            static Observations<State, Action, Reward> readFromFile(std::istream& input);
            std::vector<TraceType>& getTraces();
        };
    }
}
