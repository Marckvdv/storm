#include "src/logic/AtomicExpressionFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        AtomicExpressionFormula::AtomicExpressionFormula(storm::expressions::Expression const& expression) : expression(expression) {
            // Intentionally left empty.
        }
        
        bool AtomicExpressionFormula::isAtomicExpressionFormula() const {
            return true;
        }
        
        boost::any AtomicExpressionFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        storm::expressions::Expression const& AtomicExpressionFormula::getExpression() const {
            return expression;
        }
        
        void AtomicExpressionFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            atomicExpressionFormulas.push_back(std::dynamic_pointer_cast<AtomicExpressionFormula const>(this->shared_from_this()));
        }
        
        std::shared_ptr<Formula> AtomicExpressionFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<AtomicExpressionFormula>(this->expression.substitute(substitution));
        }
        
        std::ostream& AtomicExpressionFormula::writeToStream(std::ostream& out) const {
            out << expression;
            return out;
        }
    }
}