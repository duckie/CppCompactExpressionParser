/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_INTERFACES_H_
#define CEP_INTERFACES_H_

#include <vector>
#include <boost/shared_ptr.hpp>

namespace CompactExpressionParser
{

struct Function
{
	typedef boost::shared_ptr<Function> FuncPtr;
	virtual double operator()(const std::vector<double> iArgs) = 0;
};

}

#endif /* CEP_INTERFACES_H_ */
