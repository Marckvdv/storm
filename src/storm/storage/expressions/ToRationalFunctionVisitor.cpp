#include "storm/storage/expressions/ToRationalFunctionVisitor.h"

#include <sstream>

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {

#ifdef STORM_HAVE_CARL
        template<typename RationalFunctionType>
        ToRationalFunctionVisitor<RationalFunctionType>::ToRationalFunctionVisitor(ExpressionEvaluatorBase<RationalFunctionType> const& evaluator) : ExpressionVisitor(), cache(new carl::Cache<carl::PolynomialFactorizationPair<RawPolynomial>>()), evaluator(evaluator) {
            // Intentionally left empty.
        }
        
        template<typename RationalFunctionType>
        RationalFunctionType ToRationalFunctionVisitor<RationalFunctionType>::toRationalFunction(Expression const& expression) {
            return boost::any_cast<RationalFunctionType>(expression.accept(*this, boost::none));
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(IfThenElseExpression const& expression, boost::any const& data) {
            bool conditionValue = evaluator.asBool(expression.getCondition());
            if (conditionValue) {
                return expression.getThenExpression()->accept(*this, data);
            } else {
                return expression.getElseExpression()->accept(*this, data);
            }
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryBooleanFunctionExpression const&, boost::any const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            RationalFunctionType firstOperandAsRationalFunction = boost::any_cast<RationalFunctionType>(expression.getFirstOperand()->accept(*this, data));
            RationalFunctionType secondOperandAsRationalFunction = boost::any_cast<RationalFunctionType>(expression.getSecondOperand()->accept(*this, data));
            uint_fast64_t exponentAsInteger = 0;
            switch(expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    return firstOperandAsRationalFunction + secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    return firstOperandAsRationalFunction - secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    return firstOperandAsRationalFunction * secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    return firstOperandAsRationalFunction / secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    STORM_LOG_THROW(storm::utility::isInteger(secondOperandAsRationalFunction), storm::exceptions::InvalidArgumentException, "Exponent of power operator must be a positive integer.");
                    exponentAsInteger = storm::utility::convertNumber<uint_fast64_t>(secondOperandAsRationalFunction);
                    return storm::utility::pow(firstOperandAsRationalFunction, exponentAsInteger);
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Illegal operator type.");
            }
            
            // Return a dummy. This point must, however, never be reached.
            return boost::any();
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryRelationExpression const&, boost::any const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(VariableExpression const& expression, boost::any const&) {
            auto valueIt = valueMapping.find(expression.getVariable());
            if (valueIt != valueMapping.end()) {
                return valueIt->second;
            }
            
            auto variablePair = variableToVariableMap.find(expression.getVariable());
            if (variablePair != variableToVariableMap.end()) {
                return convertVariableToPolynomial(variablePair->second);
            } else {
                carl::Variable carlVariable = carl::freshRealVariable(expression.getVariableName());
                variableToVariableMap.emplace(expression.getVariable(), carlVariable);
                return convertVariableToPolynomial(carlVariable);
            }
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(UnaryBooleanFunctionExpression const&, boost::any const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(UnaryNumericalFunctionExpression const&, boost::any const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BooleanLiteralExpression const&, boost::any const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(IntegerLiteralExpression const& expression, boost::any const&) {
            return RationalFunctionType(storm::utility::convertNumber<storm::RationalFunction>(expression.getValue()));
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(RationalLiteralExpression const& expression, boost::any const&) {
            return storm::utility::convertNumber<storm::RationalFunction>(expression.getValue());
        }
        
        template<typename RationalFunctionType>
        void ToRationalFunctionVisitor<RationalFunctionType>::setMapping(storm::expressions::Variable const& variable, RationalFunctionType const& value) {
            valueMapping[variable] = value;
        }

        template class ToRationalFunctionVisitor<storm::RationalFunction>;
#endif
    }
}
