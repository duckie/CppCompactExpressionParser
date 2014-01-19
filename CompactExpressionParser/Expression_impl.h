/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_EXPRESSION_H_
#define CEP_EXPRESSION_H_

#include "Interfaces.h"
#include "SpiritParserDefinition.hpp"
#include <boost/lambda/lambda.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <functional>
#include <string>

namespace CompactExpressionParser
{
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace lambda = boost::lambda;
using namespace std::placeholders;

template <typename Iterator, typename T> struct ExpGrammar;
struct Unit;
struct ExpressionCalculator;

template <typename T> class Expression {
	boost::shared_ptr< ExpGrammar<std::string::const_iterator, T> > m_grammar;
	boost::shared_ptr< ExpressionCalculator > m_calculator;
	boost::scoped_ptr< Unit > m_result;
	std::string::const_iterator m_iter, m_end;

public:
	Expression() :
    m_grammar(new ExpGrammar<std::string::const_iterator>),
    m_calculator(new ExpressionCalculator),
    m_result(new Unit)
  {}

	Expression(const Expression& iExp):
    m_grammar(iExp.m_grammar),
    m_calculator(iExp.m_calculator),
    m_result(new Unit(*(iExp.m_result)))
  {}

	virtual ~Expression() {}

	bool compile(const std::string& iExpression) {
    m_iter = iExpression.begin(); m_end = iExpression.end();
    return (phrase_parse(m_iter, m_end, *m_grammar, boost::spirit::ascii::space, *m_result) && m_iter == m_end);
  }

	T eval() { return (*m_calculator)(*m_result); }

	T operator() () { return eval(); }

	bool register_function(const std::string& iName, UserFunctionType<T> iFunc) { 
    return m_grammar->addFunction(iName,iFunc);
  }
};

template <typename T> class RuntimeFunction
{
	std::string m_name;
	std::string m_arg_reader_name;
	Expression<T> m_Exp;
	const std::vector< ResultType<T> > * m_dynamic_args;

	ResultType<T> ArgumentGetter(const std::vector< ResultType<T> >& iIndex) {
    return m_dynamic_args->at(static_cast<size_t>(iIndex[0]) - 1);
  }

public:
	RuntimeFunction(const RuntimeFunction&) = delete;
	RuntimeFunction& operator= (const RuntimeFunction&) = delete;
	RuntimeFunction(Expression& iExp, const std::string& iName)
    : m_Exp(iExp), m_name(iName)
  {
    std::ostringstream arg_reader;
    arg_reader << "_Binder_" << m_name;
    m_arg_reader_name = arg_reader.str();
    m_Exp.register_function(m_arg_reader_name,std::bind(std::mem_fn(&RuntimeFunction::ArgumentGetter),std::ref(*this),_1));
    iExp.register_function(m_name,std::ref(*this));
  }

	bool compile(const std::string& iStringExpr) {
    std::ostringstream m_output;
    std::string::const_iterator iter = iStringExpr.begin(); std::string::const_iterator end = iStringExpr.end();
    bool func_parsing = qi::parse(iter,end,
        *( (qi::char_ - '_')[ m_output << qi::_1 ] | '_' >> qi::int_[ lambda::var(m_output) << m_arg_reader_name << '(' << lambda::_1 << ')' ] )
        );
    if(func_parsing && iter == end) return m_Exp.compile(m_output.str());
    else return false;
  }

	ResultType operator()(const std::vector<ResultType>& args) {
    m_dynamic_args = &args; 
    return m_Exp();
  }
};

}

#endif /* CEP_EXPRESSION_H_ */
