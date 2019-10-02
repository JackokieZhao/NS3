/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#pragma once

#include "attribute.h"
#include "attribute-helper.h"

/**
 * \file
 * \ingroup attribute_Boolean
 * ns3::BooleanValue attribute value declarations.
 */

namespace ns3 {

// Doxygen for this class is auto-generated by
// utils/print-introspected-doxygen.h
class BooleanValue : public AttributeValue
{
public:
  BooleanValue ();
  /**
   * Construct from an explicit value.
   *
   * \param [in] value The boolean value to begin with.
   */
  BooleanValue (bool value);
  void Set (bool value);
  bool Get (void) const;
  template <typename T>
  bool GetAccessor (T &v) const;

  /**
   * Functor returning the value.
   * \returns The value.
   */
  operator bool () const;

  virtual Ptr<AttributeValue> Copy (void) const;
  virtual stdfwd::string SerializeToString (Ptr<const AttributeChecker> checker) const;
  virtual bool DeserializeFromString (stdfwd::string value, Ptr<const AttributeChecker> checker);
private:
  bool m_value;
};

template <typename T>
bool BooleanValue::GetAccessor (T &v) const
{
  v = T (m_value);
  return true;
}

/**
 * \ingroup attribute_Boolean
 * Output streamer.
 *
 * The value is printed as "true" or "false".
 *
 * \param [in,out] os The stream.
 * \param [in] value The BooleanValue to print.
 * \returns The stream.
 */
std::ostream & operator << (std::ostream &os, const BooleanValue &value);

ATTRIBUTE_CHECKER_DEFINE (Boolean);
ATTRIBUTE_ACCESSOR_DEFINE (Boolean);

} // namespace ns3


