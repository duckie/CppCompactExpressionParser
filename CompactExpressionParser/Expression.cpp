/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#include "Expression.h"
#include "SpiritParserDefinition.hpp"

namespace CompactExpressionParser
{

Expression::Expression() :
m_grammar(new ExpGrammar<std::string::const_iterator>),
m_calculator(new ExpressionCalculator),
m_result(new Unit)
{}

Expression::Expression(const Expression& iExp) :
m_grammar(iExp.m_grammar),
m_calculator(iExp.m_calculator),
m_result(new Unit(*(iExp.m_result)))
{}

Expression::~Expression(){}

bool Expression::compile(const std::string& iExpression)
{
	m_iter = iExpression.begin(); m_end = iExpression.end();
	return (phrase_parse(m_iter, m_end, *m_grammar, boost::spirit::ascii::space, *m_result) && m_iter == m_end);
}

double Expression::eval() { return (*m_calculator)(*m_result); }
double Expression::operator() () { return eval(); }

bool Expression::register_user_function(const std::string& iName, UserFunctionType iFunc)
{
	return m_grammar->addFunction(iName,iFunc);
}

}
