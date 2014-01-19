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
#include <boost/function.hpp>
#include <boost/variant.hpp>

namespace CompactExpressionParser {
class ResultType { 
  boost::variant<double, std::string> value_;

 public:
  ResultType() {}
  ResultType(double value) : value_(value) {}
  ResultType(std::string const& value) : value_(value) {}
  ResultType& operator= (double value) { value_ = value; return *this; }
  ResultType& operator= (std::string const& value) { value_ = value; return *this; }
  operator double () const { 
    double const * pv = boost::get<double>(&value_);
    return pv ? *pv : 0.;
  }
  operator std::string () const { 
    std::string const * pv = boost::get<std::string>(&value_);
    return pv ? *pv : "fuck";
  }
};

typedef boost::function< ResultType (const std::vector<ResultType>&) > UserFunctionType;
}  // namespace CompactExpressionParser

#endif /* CEP_INTERFACES_H_ */
