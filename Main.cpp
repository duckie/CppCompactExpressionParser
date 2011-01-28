/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <CompactExpressionParser/Expression.h>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ref.hpp>

#define add_example(A,B) cep_add_example(A,B);

// Here are the definitions of user functions that will be used later

struct Pi
{
	double operator()(const std::vector<double>& args)
	{
		static double pi = std::atan2(0.,-1.);
		return pi;
	}
};

struct Cosinus { double operator()(const std::vector<double>& args) { return std::cos(args[0]); } };
struct Sinus { double operator()(const std::vector<double>& args) { return std::sin(args[0]); } };
struct Arctan2 { double operator()(const std::vector<double>& args) { return std::atan2(args[0],args[1]); } };

// Unleash the power of spirit : see the use of UserArg below
struct UserArg
{
	double m_value;
	UserArg() : m_value(0.){}
	UserArg(const double& iValue) : m_value(iValue) {}
	UserArg(const UserArg& iArg) : m_value(iArg.m_value) {}
	UserArg& operator=(const UserArg& iArg) { m_value = iArg.m_value; return *this; }
	UserArg& operator=(const double& iValue) { m_value = iValue; return *this; }
	double operator()(const std::vector<double>& args) { return m_value; }
};


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>

namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace lambda = boost::lambda;

struct UserFunc
{
	CompactExpressionParser::Expression m_Exp;
	const std::vector<double> * m_dynamic_args;

	double ArgumentGetter(const std::vector<double>& iIndex)
	{
		return m_dynamic_args->at(static_cast<unsigned int>(iIndex[0]) - 1);
	}

	UserFunc(std::string iName, std::string iStringExpr)
	{
		std::ostringstream arg_reader;
		arg_reader << "_Binder_" << iName;
		std::string arg_reader_name = arg_reader.str();
		m_Exp.register_function(arg_reader_name,boost::bind(&UserFunc::ArgumentGetter,this,::_1));

		std::ostringstream m_output;
		std::string::const_iterator iter = iStringExpr.begin(); std::string::const_iterator end = iStringExpr.end();
		bool func_parsing = qi::parse(iter,end,
			*( (qi::char_ - '_')[ m_output << lambda::_1 ] | '_' >> qi::int_[ lambda::var(m_output) << arg_reader_name << '(' << lambda::_1 << ')' ] )
		);

		std::cout << "Parsed: " << m_output.str() << std::endl;
		bool succes = m_Exp.compile(m_output.str());
		std::cout << "Compiled: " << succes << std::endl;
	}

	double operator()(const std::vector<double>& args) {
		m_dynamic_args = &args;
		return m_Exp();
	}
};

// Some convenient utilities for the main loop
void cep_example_output(int index, double value)
{
	std::cout << "[Ex" << index << "] The result is : " << value << std::endl;
}

void cep_add_example(int& index, const std::string& iName = "")
{
	index++;
	std::cout << std::endl;
	std::cout << "Example " << index << ": " << iName << std::endl;
}

// Check out the examples here !
int main()
{
	using namespace CompactExpressionParser;
	int index_example = 0;

	add_example(index_example,"The most simple example")
	{
		Expression exp;
		exp.compile("4+3*2"); // Notice that the operands priorities are supported
		cep_example_output(index_example, exp() );
		exp.compile("4 + 3 * 2 / 13.21^(-1.) - (25-2)*1.7"); // Feel free to use spaces
		cep_example_output(index_example, exp() );
	}

	add_example(index_example,"Testing the parsing success")
	{
		Expression exp;
		bool status = exp.compile("4+3*2^(6+3)"); // Success
		cep_example_output(index_example, status ? 1. : 0. );
		status = exp.compile("4++-1,5*/1523"); // Fail
		cep_example_output(index_example, status ? 1. : 0. );
	}

	add_example(index_example,"Using some additional functions")
	{
		Expression exp;
		exp.register_function("Pi", Pi() );
		exp.register_function("cos", Cosinus() );
		exp.register_function("sin", Sinus() );
		exp.register_function("atan2", Arctan2() );

		exp.compile("sin(Pi()/2)");
		cep_example_output(index_example, exp() );

		exp.compile("atan2(0,-1)");
		cep_example_output(index_example, exp() );

		exp.compile("cos(2*Pi()/3)");
		cep_example_output(index_example, exp() );
	}

	add_example(index_example,"Testing function registering success")
	{
		Expression exp;
		bool status = exp.register_function("Salut34Roger", Pi() ); // Success
		cep_example_output(index_example, status ? 1. : 0. );
		status = exp.register_function("_12/Invalid?Func+Name", Pi() ); // Fail
		cep_example_output(index_example, status ? 1. : 0. );
	}


	add_example(index_example,"Using user arguments (NOOB version)")
	{
		std::string string_raw_exp("4 + 3*%1% - %2%");
		double user_arg1 = 2.;
		double user_arg2 = 5.;
		std::string string_expr = boost::str(boost::format(string_raw_exp) % user_arg1 % user_arg2);

		Expression exp;
		exp.compile(string_expr);
		cep_example_output(index_example, exp() );
	}

	add_example(index_example,"Using user arguments (MASTER version)")
	{
		// This example is important cause it shows the ability not to recompile an expression

		// Prepare the expression
		UserArg arg1;
		UserArg arg2;
		Expression exp;
		exp.register_function("Arg1", boost::ref(arg1)); // Be sure to use a boost::ref so arg1 and arg2
		exp.register_function("Arg2", boost::ref(arg2)); // can be modified AFTER compilation
		exp.compile("4 + 3*Arg1() - Arg2()");
		cep_example_output(index_example, exp() );

		// Notice that you DONT have to compile the expression again
		arg1 = 2.; arg2 = 5.;
		cep_example_output(index_example, exp() );

		arg1 = 3.; arg2 = 1.;
		cep_example_output(index_example, exp() );

		cep_add_example(index_example,"Re-use user arguments (WRONG way)");
		Expression exp2;
		bool status = exp2.compile("4 + 3*Arg1() - Arg2()"); // This will fail cause Arg1 and Arg2 are not registered
		cep_example_output(index_example, status ? 1. : 0. );

		cep_add_example(index_example,"Re-use user arguments (RIGHT way)");
		Expression exp3(exp); // <- exp3 will get the same register than exp, still being ANOTHER expression
		exp3.compile("Arg1() + Arg2()"); // This will success cause Arg1 and Arg2 are registered

		cep_example_output(index_example, exp() ); // Now you can use both and change args
		cep_example_output(index_example, exp3() );// without recompilation
	}

	add_example(index_example,"Using runtime defined functions")
	{
		UserFunc roger("roger","_1 + _2 * _3");
		Expression exp;
		exp.register_function("roger",boost::ref(roger));
		exp.compile("roger(1,2,2)");

		cep_example_output(index_example, exp() );
	}

	return 0;
}

