/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_INTERFACES_H_
#define CEP_INTERFACES_H_

#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <functional>

namespace CompactExpressionParser {
class ResultType { 
  boost::variant<double, std::string> value_;

 public:
  ResultType();
  ResultType(double value);
  ResultType(std::string const& value);
  ResultType& operator= (double value);
  ResultType& operator= (std::string const& value);
  bool IsNumber() const;
  operator double () const;
  operator std::string () const;
};

typedef std::function< ResultType (const std::vector<ResultType>&) > UserFunctionType;
}  // namespace CompactExpressionParser

#endif /* CEP_INTERFACES_H_ */
