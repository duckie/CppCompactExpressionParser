/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_INTERFACES_H_
#define CEP_INTERFACES_H_

#include <vector>
#include <boost/function.hpp>

namespace CompactExpressionParser
{
typedef boost::function< double (const std::vector<double>&) > UserFunctionType;
}

#endif /* CEP_INTERFACES_H_ */
