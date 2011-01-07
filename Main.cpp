/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#include <iostream>
#include <cmath>
#include <CompactExpressionParser/Expression.h>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ref.hpp>

#define add_example(A) cep_add_example(A);

// Here are the definitions of user functions that will be used later

struct Pi
{
	double operator()(const std::vector<double> args)
	{
		static double pi = std::atan2(0.,-1.);
		return pi;
	}
};

struct Cosinus { double operator()(const std::vector<double> args) { return std::cos(args[0]); } };
struct Sinus { double operator()(const std::vector<double> args) { return std::sin(args[0]); } };
struct Arctan2 { double operator()(const std::vector<double> args) { return std::atan2(args[0],args[1]); } };

// Unleash the power of spirit : see the use of UserArg below
struct UserArg
{
	double m_value;
	UserArg() : m_value(0.){}
	UserArg(const double& iValue) : m_value(iValue) {}
	UserArg(const UserArg& iArg) : m_value(iArg.m_value) {}
	UserArg& operator=(const UserArg& iArg) { m_value = iArg.m_value; return *this; }
	UserArg& operator=(const double& iValue) { m_value = iValue; return *this; }
	double operator()(const std::vector<double> args) { return m_value; }
};

// Some convenient utilities for the main loop
void cep_example_output(int index, double value)
{
	std::cout << "[Ex" << index << "] The result is : " << value << std::endl;
}

void cep_add_example(int& index)
{
	index++;
	std::cout << std::endl;
	std::cout << "Beginning example " << index << std::endl;
}

// Check out the examples here !
int main()
{
	using namespace CompactExpressionParser;
	int index_example = 0;

	// The most simple example
	add_example(index_example)
	{
		Expression exp;
		exp.compile("4+3*2"); // Notice that the operands priorities are supported
		cep_example_output(index_example, exp() );
		exp.compile("4 + 3 * 2 / 13.21^(-1.) - (25-2)*1.7"); // Feel free to use spaces
		cep_example_output(index_example, exp() );
	}

	// Testing the parsing success
	add_example(index_example)
	{
		Expression exp;
		bool status = exp.compile("4+3*2^(6+3)"); // Success
		cep_example_output(index_example, status ? 1. : 0. );
		status = exp.compile("4++-1,5*/1523"); // Fail
		cep_example_output(index_example, status ? 1. : 0. );
	}

	// Using some additional functions
	add_example(index_example)
	{
		Expression exp;
		exp.register_user_function("Pi", Pi() );
		exp.register_user_function("cos", Cosinus() );
		exp.register_user_function("sin", Sinus() );
		exp.register_user_function("atan2", Arctan2() );

		exp.compile("sin(Pi()/2)");
		cep_example_output(index_example, exp() );

		exp.compile("atan2(0,-1)");
		cep_example_output(index_example, exp() );

		exp.compile("cos(2*Pi()/3)");
		cep_example_output(index_example, exp() );
	}

	// Using user arguments : way of the NOOB
	add_example(index_example)
	{
		std::string string_raw_exp("4 + 3*%1% - %2%");
		double user_arg1 = 2.;
		double user_arg2 = 5.;
		std::string string_expr = boost::str(boost::format(string_raw_exp) % user_arg1 % user_arg2);

		Expression exp;
		exp.compile(string_expr);
		cep_example_output(index_example, exp() );
	}

	// Using user arguments : way of the MASTER
	// This example is important cause it shows the ability not to recompile an expression
	add_example(index_example)
	{
		// Prepare the expression
		UserArg arg1;
		UserArg arg2;
		Expression exp;
		exp.register_user_function("Arg1", boost::ref(arg1)); // Be sure to use a boost::ref so arg1 and arg2
		exp.register_user_function("Arg2", boost::ref(arg2)); // can be modified AFTER compilation
		exp.compile("4 + 3*Arg1() - Arg2()");

		// Lets use it. Dont forget to think to the args as pointers
		cep_example_output(index_example, exp() );

		// Notice that you DONT have to compile the expression again
		arg1 = 2.; arg2 = 5.;
		cep_example_output(index_example, exp() );

		arg1 = 3.; arg2 = 1.;
		cep_example_output(index_example, exp() );

		// Let's reuse these : the WRONG way
		cep_add_example(index_example);
		Expression exp2;
		bool status = exp2.compile("4 + 3*Arg1() - Arg2()"); // This will fail cause Arg1 and Arg2 are not registered
		cep_example_output(index_example, status ? 1. : 0. );

		// Let's reuse these : the RIGHT way
		cep_add_example(index_example);
		Expression exp3(exp); // <- exp3 will get the same register than exp, still being ANOTHER expression
		exp3.compile("Arg1() + Arg2()"); // This will success cause Arg1 and Arg2 are registered

		cep_example_output(index_example, exp() ); // Now you can use both and change args
		cep_example_output(index_example, exp3() );// without recompilation
	}

	return 0;
}

