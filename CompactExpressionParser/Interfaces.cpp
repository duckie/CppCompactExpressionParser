/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

/** @author: Jean-Bernard Jansen <jeanbernard@jjansen.fr> */
#include "Interfaces.h"

namespace CompactExpressionParser {

ResultType::ResultType() {}
ResultType::ResultType(double value) : value_(value) {}
ResultType::ResultType(std::string const& value) : value_(value) {}
ResultType& ResultType::operator= (double value) { value_ = value; return *this; }
ResultType& ResultType::operator= (std::string const& value) { value_ = value; return *this; }
bool ResultType::IsNumber() const { return nullptr != boost::get<double>(&value_); }

ResultType::operator double () const { 
  double const * pv = boost::get<double>(&value_);
  return pv ? *pv : 0.;
}

ResultType::operator std::string () const { 
  std::string const * pv = boost::get<std::string>(&value_);
  return pv ? *pv : "";
}

};  // namespace CompactExpressionParser

