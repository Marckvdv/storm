#ifndef STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_
#define STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class ProbabilityOperatorFormula : public OperatorFormula {
        public:
            ProbabilityOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation = OperatorInformation());

            virtual ~ProbabilityOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isProbabilityOperatorFormula() const override;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_ */