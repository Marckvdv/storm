/* 
 * File:   BooleanExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:27 PM
 */

#ifndef BOOLEANEXPRESSIONGRAMMAR_H
#define	BOOLEANEXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "VariableState.h"
#include "IdentifierGrammars.h"
#include "Tokens.h"

#include <iostream>

namespace storm {
namespace parser {
namespace prism {

class BooleanExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused>, public BaseGrammar<BooleanExpressionGrammar> {
public:
	BooleanExpressionGrammar(std::shared_ptr<VariableState>& state);

private:
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused> booleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> orExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> andExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> notExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> atomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> relativeExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanVariableExpression;

	storm::parser::prism::relationalOperatorStruct relations_;
};


}
}
}

#endif	/* BOOLEANEXPRESSIONGRAMMAR_H */
