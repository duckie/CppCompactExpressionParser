/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */

#ifndef CEP_INTERFACES_IMPL_H_
#define CEP_INTERFACES_IMPL_H_

#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <functional>

namespace CompactExpressionParser {
template <typename T> class ResultType { 
  boost::variant<T, std::string> value_;

 public:
  ResultType() {}
  ResultType(T value) : value_(value) {}
  ResultType(std::string const& value) : value_(value) {}
  ResultType& operator= (T value) { value_ = value; return *this; }
  ResultType& operator= (std::string const& value) { value_ = value; return *this; }
  bool IsNumber() const { return nullptr != boost::get<T>(&value_); }
  operator T () const { 
    T const * pv = boost::get<T>(&value_);
    return pv ? *pv : 0.;
  }
  operator std::string () const { 
    std::string const * pv = boost::get<std::string>(&value_);
    return pv ? *pv : "";
  }
};

template <typename T> using UserFunctionType = std::function< ResultType<T> (const std::vector< ResultType<T> >&) >;
}  // namespace CompactExpressionParser

#endif /* CEP_INTERFACES_IMPL_H_ */
