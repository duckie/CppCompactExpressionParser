/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_EXPRESSION_H_
#define CEP_EXPRESSION_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "Interfaces.h"

namespace CompactExpressionParser
{

template <typename Iterator> struct ExpGrammar;
struct Unit;
struct ExpressionCalculator;

class Expression {
public:
	Expression();
	Expression(const Expression& iExp);
	virtual ~Expression();
	bool compile(const std::string& iExpression);
	double eval();
	double operator() ();
	bool register_function(const std::string& iName, UserFunctionType iFunc);

private:
	boost::shared_ptr< ExpGrammar<std::string::const_iterator> > m_grammar;
	boost::shared_ptr< ExpressionCalculator > m_calculator;
	boost::scoped_ptr< Unit > m_result;
	std::string::const_iterator m_iter, m_end;
};

class RuntimeFunction
{
public:
	RuntimeFunction(Expression& iExp, const std::string& iName);
	bool compile(const std::string& iStringExpr);
	double operator()(const std::vector<double>& args);

private:
	RuntimeFunction(const RuntimeFunction&);
	RuntimeFunction& operator= (const RuntimeFunction&);
	double ArgumentGetter(const std::vector<double>& iIndex);

	std::string m_name;
	std::string m_arg_reader_name;
	Expression m_Exp;
	const std::vector<double> * m_dynamic_args;
};

}

#endif /* CEP_EXPRESSION_H_ */
