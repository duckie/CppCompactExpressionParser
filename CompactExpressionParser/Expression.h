/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_EXPRESSION_H_
#define CEP_EXPRESSION_H_

#include <string>
#include <memory>
#include <boost/variant/variant_fwd.hpp>
#include <tuple>
#include "Interfaces.h"

namespace CompactExpressionParser
{

template<typename T> struct Operation;
struct add; struct sub;
struct mult; struct divide;
struct power;
struct FunctionCall;

typedef boost::make_recursive_variant< 
    double,
    std::string,
		std::tuple<boost::recursive_variant_>,
		boost::recursive_wrapper< Operation<add> >,
		boost::recursive_wrapper< Operation<sub> >,
		boost::recursive_wrapper< Operation<mult> >,
		boost::recursive_wrapper< Operation<divide> >,
		boost::recursive_wrapper< Operation<power> >,
		boost::recursive_wrapper< FunctionCall >
> ExpressionVar;

using Unit = std::tuple<ExpressionVar>;

template <typename Iterator> struct ExpGrammar;
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
	std::shared_ptr< ExpGrammar<std::string::const_iterator> > m_grammar;
	std::shared_ptr< ExpressionCalculator > m_calculator;
	std::unique_ptr< Unit > m_result;
	std::string::const_iterator m_iter, m_end;
};

class RuntimeFunction
{
public:
	RuntimeFunction(Expression& iExp, const std::string& iName);
	bool compile(const std::string& iStringExpr);
	ResultType operator()(const std::vector<ResultType>& args);

private:
	RuntimeFunction(const RuntimeFunction&);
	RuntimeFunction& operator= (const RuntimeFunction&);
	ResultType ArgumentGetter(const std::vector<ResultType>& iIndex);

	std::string m_name;
	std::string m_arg_reader_name;
	Expression m_Exp;
	const std::vector<ResultType> * m_dynamic_args;
};

}

#endif /* CEP_EXPRESSION_H_ */
