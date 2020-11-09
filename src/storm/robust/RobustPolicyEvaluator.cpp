#include "storm/robust/RobustPolicyEvaluator.h"

namespace storm {
    namespace robust {
        template <typename ValueType, typename State, typename Action>
        RobustPolicyEvaluator<ValueType, State, Action>::RobustPolicyEvaluator(AmbiguitySet<ValueType, State, Action>) :
            ambiguitySet(ambiguitySet) {
        }

        template <typename ValueType, typename State, typename Action>
        ValueType RobustPolicyEvaluator<ValueType, State, Action>::estimateWorstCaseReward() {
            /*
            GRBEnv env(true);
            GRBModel model(env);

            // Create variables
            std::vector<GRBVar> xi;
            for (size_t i = 0; i < ambiguitySet.dimensions; ++i) {
                xi.push_back(model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS);
            }
            */
        }
    }
}
