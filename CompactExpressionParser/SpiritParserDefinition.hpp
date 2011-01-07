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

			glob = exp [ at_c<0>(_val) = _1 ];
			exp = (opAdd | opSub) [ _val = _1 ] | exp2 [ _val = _1 ];
			exp2 = (opMult | opDiv) [ _val = _1 ] | exp3 [ _val = _1 ];
			exp3 = opPower [ _val = _1 ] | value [ _val = _1 ];
			value = (double_ | function | functionWithArgs) [ _val = _1] | group [_val = _1];
			group = '(' >> exp [ _val = _1] >> ')';
			function = functionsTable_ [ at_c<0>(_val) = _1 ] >> '(' >> ')';
			functionWithArgs = functionsTable_ [ at_c<0>(_val) = _1 ] >> arglist [ at_c<1>(_val) = _1 ];
			arglist = '(' >> exp [push_back(_val,_1) ] >> *( ',' >> exp [push_back(_val,_1) ] ) >> ')';
			opAdd = exp2 [at_c<0>(_val) = _1] >> '+' >> exp [at_c<1>(_val) = _1];
			opSub = exp2 [at_c<0>(_val) = _1] >> '-' >> exp [at_c<1>(_val) = _1];
			opMult = exp3 [at_c<0>(_val) = _1] >> '*' >> exp2 [at_c<1>(_val) = _1];
			opDiv = exp3 [at_c<0>(_val) = _1] >> '/' >> exp2 [at_c<1>(_val) = _1];
			opPower = value [at_c<0>(_val) = _1] >> '^' >> exp3 [at_c<1>(_val) = _1];
		}

		void addFunction(const std::string& iName, UserFunctionType iFunc)
		{
			if(iName.size() > 0) // Trivial check for a valid function name
				functionsTable_.add(iName,iFunc);
		}

		qi::rule<Iterator, Unit(), ascii::space_type> glob;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> exp;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> exp2;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> exp3;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> value;
		qi::rule<Iterator, ExpressionVar(), ascii::space_type> group;
		qi::rule<Iterator, FunctionCall(), ascii::space_type> function;
		qi::rule<Iterator, FunctionCall(), ascii::space_type> functionWithArgs;
		qi::rule<Iterator, std::vector<ExpressionVar>(), ascii::space_type> arglist;
		qi::rule<Iterator, Operation<add>(), ascii::space_type> opAdd;
		qi::rule<Iterator, Operation<sub>(), ascii::space_type> opSub;
		qi::rule<Iterator, Operation<mult>(), ascii::space_type> opMult;
		qi::rule<Iterator, Operation<divide>(), ascii::space_type> opDiv;
		qi::rule<Iterator, Operation<power>(), ascii::space_type> opPower;

		qi::symbols<char, UserFunctionType > functionsTable_;
	};

	struct ExpressionCalculator : boost::static_visitor<double>
	{
		ExpressionCalculator(){}
		double Evaluate(const ExpressionVar& iExp) const { return boost::apply_visitor(*this, iExp); }
		double operator()(const FunctionCall& iFunc) const;
		double operator()(const Unit& iUnit) const { return Evaluate(iUnit.value); }
		double operator()(const double& ivalue) const { return ivalue; }
		template <typename T> double operator()(Operation<T> const& iExp) const;
	};

	template<> double ExpressionCalculator::operator()(Operation<add> const& iExp) const { return Evaluate(iExp.opLeft) + Evaluate(iExp.opRight); }
	template<> double ExpressionCalculator::operator()(Operation<sub> const& iExp) const { return Evaluate(iExp.opLeft) - Evaluate(iExp.opRight); }
	template<> double ExpressionCalculator::operator()(Operation<mult> const& iExp) const { return Evaluate(iExp.opLeft) * Evaluate(iExp.opRight); }
	template<> double ExpressionCalculator::operator()(Operation<divide> const& iExp) const { return Evaluate(iExp.opLeft) / Evaluate(iExp.opRight); }
	template<> double ExpressionCalculator::operator()(Operation<power> const& iExp) const { return std::pow(Evaluate(iExp.opLeft),Evaluate(iExp.opRight)); }
	double ExpressionCalculator::operator()(const FunctionCall& iFunc) const
	{
		std::vector<double> args;
		BOOST_FOREACH(const ExpressionVar& e, iFunc.units) args.push_back(Evaluate(e));
		return iFunc.func(args);
	}
}

#endif /* CEP_COMPACTEXPRESSIONPARSER_HPP_ */
