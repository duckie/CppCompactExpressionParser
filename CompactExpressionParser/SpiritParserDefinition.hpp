/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_COMPACTEXPRESSIONPARSER_HPP_
#define CEP_COMPACTEXPRESSIONPARSER_HPP_

#include <cmath>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/variant.hpp>

#include "Interfaces.h"

namespace CompactExpressionParser
{
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename T> struct Operation;
struct add; struct sub;
struct mult; struct divide;
struct power;
struct Unit;
struct FunctionCall;

typedef boost::variant< double,
    std::string,
		boost::recursive_wrapper< Unit >,
		boost::recursive_wrapper< Operation<add> >,
		boost::recursive_wrapper< Operation<sub> >,
		boost::recursive_wrapper< Operation<mult> >,
		boost::recursive_wrapper< Operation<divide> >,
		boost::recursive_wrapper< Operation<power> >,
		boost::recursive_wrapper< FunctionCall >
> ExpressionVar;

template<typename T> struct Operation { ExpressionVar opLeft; ExpressionVar opRight; };
struct Unit { ExpressionVar value; };
struct FunctionCall { UserFunctionType func; std::vector<ExpressionVar> units; };
}

namespace { namespace CEP = CompactExpressionParser ; }
BOOST_FUSION_ADAPT_STRUCT( CEP::Unit, (CEP::ExpressionVar, value))
BOOST_FUSION_ADAPT_STRUCT( CEP::FunctionCall, (CEP::UserFunctionType, func) (std::vector<CEP::ExpressionVar>, units))
BOOST_FUSION_ADAPT_STRUCT( CEP::Operation<CEP::add>, (CEP::ExpressionVar, opLeft) (CEP::ExpressionVar, opRight))
BOOST_FUSION_ADAPT_STRUCT( CEP::Operation<CEP::sub>, (CEP::ExpressionVar, opLeft) (CEP::ExpressionVar, opRight))
BOOST_FUSION_ADAPT_STRUCT( CEP::Operation<CEP::mult>, (CEP::ExpressionVar, opLeft) (CEP::ExpressionVar, opRight))
BOOST_FUSION_ADAPT_STRUCT( CEP::Operation<CEP::divide>, (CEP::ExpressionVar, opLeft) (CEP::ExpressionVar, opRight))
BOOST_FUSION_ADAPT_STRUCT( CEP::Operation<CEP::power>, (CEP::ExpressionVar, opLeft) (CEP::ExpressionVar, opRight))

namespace CompactExpressionParser
{
	template <typename Iterator> struct ExpGrammar : qi::grammar<Iterator, Unit(), boost::spirit::ascii::space_type>
	{
		ExpGrammar() : ExpGrammar::base_type(glob)
		{
			namespace ascii = boost::spirit::ascii;
			using namespace qi::labels;
			using qi::double_;
			using phoenix::at_c;
			using phoenix::push_back;
			using phoenix::ref;

      unesc_char.add("\\a", '\a')("\\b", '\b')("\\f", '\f')("\\n", '\n')
        ("\\r", '\r')("\\t", '\t')("\\v", '\v')
        ("\\\\", '\\')("\\\'", '\'')("\\\"", '\"')
        (" ",' ')("/",'/')
        ;

			glob = exp [ at_c<0>(_val) = qi::_1 ];
			exp = (opAdd | opSub) [ _val = qi::_1 ] | exp2 [ _val = qi::_1 ];
			exp2 = (opMult | opDiv) [ _val = qi::_1 ] | exp3 [ _val = qi::_1 ];
			exp3 = opPower [ _val = qi::_1 ] | value [ _val = qi::_1 ];
      string_value = '"' >> *(unesc_char | qi::alnum | "\\x" >> qi::hex) >> '"';
			value = (double_ | function | functionWithArgs) [ _val = qi::_1] | group [_val = qi::_1] | string_value [_val = qi::_1];
			group = '(' >> exp [ _val = qi::_1] >> ')';
			function = functionsTable_ [ at_c<0>(_val) = qi::_1 ] >> '(' >> ')';
			functionWithArgs = functionsTable_ [ at_c<0>(_val) = qi::_1 ] >> arglist [ at_c<1>(_val) = qi::_1 ];
			arglist = '(' >> exp [push_back(_val,qi::_1) ] % ',' >> ')';
			opAdd = exp2 [at_c<0>(_val) = qi::_1] >> '+' >> exp [at_c<1>(_val) = qi::_1];
			opSub = exp2 [at_c<0>(_val) = qi::_1] >> '-' >> exp [at_c<1>(_val) = qi::_1];
			opMult = exp3 [at_c<0>(_val) = qi::_1] >> '*' >> exp2 [at_c<1>(_val) = qi::_1];
			opDiv = exp3 [at_c<0>(_val) = qi::_1] >> '/' >> exp2 [at_c<1>(_val) = qi::_1];
			opPower = value [at_c<0>(_val) = qi::_1] >> '^' >> exp3 [at_c<1>(_val) = qi::_1];
		}

		bool addFunction(const std::string& iName, UserFunctionType iFunc)
		{
			std::string::const_iterator iter = iName.begin(); std::string::const_iterator end = iName.end();
			bool func_name_is_valid = ( qi::parse(iter,end, (qi::alpha | '_') >> *(qi::alnum | '_')) && iter == end);
			if(func_name_is_valid) functionsTable_.add(iName,iFunc);
			return func_name_is_valid;
		}

		qi::rule<Iterator, Unit(), ascii::space_type> glob;
		qi::rule<Iterator, std::string()> string_value;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> exp, exp2, exp3, value, group;
		qi::rule<Iterator, FunctionCall(), ascii::space_type> function, functionWithArgs;
		qi::rule<Iterator, std::vector<ExpressionVar>(), ascii::space_type> arglist;
		qi::rule<Iterator, Operation<add>(), ascii::space_type> opAdd;
		qi::rule<Iterator, Operation<sub>(), ascii::space_type> opSub;
		qi::rule<Iterator, Operation<mult>(), ascii::space_type> opMult;
		qi::rule<Iterator, Operation<divide>(), ascii::space_type> opDiv;
		qi::rule<Iterator, Operation<power>(), ascii::space_type> opPower;
		qi::symbols<char, UserFunctionType > functionsTable_;
    qi::symbols<char const, char const> unesc_char;
	};

	struct ExpressionCalculator : boost::static_visitor<ResultType>
	{
		ExpressionCalculator(){}
		double Evaluate(const ExpressionVar& iExp) const { return boost::apply_visitor(*this, iExp); }
		ResultType operator()(const FunctionCall& iFunc) const;
		ResultType operator()(const Unit& iUnit) const { return Evaluate(iUnit.value); }
		ResultType operator()(const double& ivalue) const { return ivalue; }
    ResultType operator()(const std::string& ivalue) const { return ivalue; }
		template <typename T> ResultType operator()(Operation<T> const& iExp) const;
	};

	template<> ResultType ExpressionCalculator::operator()(Operation<add> const& iExp) const { return Evaluate(iExp.opLeft) + Evaluate(iExp.opRight); }
	template<> ResultType ExpressionCalculator::operator()(Operation<sub> const& iExp) const { return Evaluate(iExp.opLeft) - Evaluate(iExp.opRight); }
	template<> ResultType ExpressionCalculator::operator()(Operation<mult> const& iExp) const { return Evaluate(iExp.opLeft) * Evaluate(iExp.opRight); }
	template<> ResultType ExpressionCalculator::operator()(Operation<divide> const& iExp) const { return Evaluate(iExp.opLeft) / Evaluate(iExp.opRight); }
	template<> ResultType ExpressionCalculator::operator()(Operation<power> const& iExp) const { return std::pow(Evaluate(iExp.opLeft),Evaluate(iExp.opRight)); }
	ResultType ExpressionCalculator::operator()(const FunctionCall& iFunc) const
	{
		std::vector<ResultType> args;
		BOOST_FOREACH(const ExpressionVar& e, iFunc.units) args.push_back(boost::apply_visitor(*this, e));
		return iFunc.func(args);
	}
}

#endif /* CEP_COMPACTEXPRESSIONPARSER_HPP_ */
