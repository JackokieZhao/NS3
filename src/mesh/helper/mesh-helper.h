/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008,2009 IITP RAS
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
 * Author: Kirill Andreev <andreev@iitp.ru>
 *         Pavel Boyko <boyko@iitp.ru>
 */


#pragma once

#include "mesh-stack-installer.h"
#include "../../wifi/model/wifi-phy-standard.h"
#include "../../core/model/object-factory.h"

namespace ns3 {

class NetDeviceContainer;
class WifiPhyHelper;
class WifiNetDevice;
class NodeContainer;

/** 
 * \ingroup dot11s
 * 
 * \brief Helper to create IEEE 802.11s mesh networks
 */
class MeshHelper
{
public:
  /**
   * Construct a MeshHelper used to make life easier when creating 802.11s networks.
   */
  MeshHelper ();

  /**
   * Destroy a MeshHelper.
   */
  ~MeshHelper ();

  /**
   * \brief Set the helper to the default values for the MAC type,  remote
   * station manager and channel policy.
   * \returns the default MeshHelper
   */
  static MeshHelper Default ();

  /**
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   *
   * All the attributes specified in this method should exist
   * in the requested mac.
   */
  void SetMacType (stdfwd::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                   stdfwd::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                   stdfwd::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                   stdfwd::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                   stdfwd::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                   stdfwd::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                   stdfwd::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                   stdfwd::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * \param type the type of ns3::WifiRemoteStationManager to create.
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   *
   * All the attributes specified in this method should exist
   * in the requested station manager.
   */
  void
  SetRemoteStationManager (stdfwd::string type,
                           stdfwd::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                           stdfwd::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                           stdfwd::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                           stdfwd::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                           stdfwd::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                           stdfwd::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                           stdfwd::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                           stdfwd::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Set PHY standard
   * \param standard the wifi phy standard
   */
  void SetStandard (enum WifiPhyStandard standard);
  /// \todo SetMeshId 
  //void SetMeshId (stdfwd::string s);
  /** 
   *  \brief Spread/not spread frequency channels of MP interfaces. 
   * 
   *  If set to true different non-overlapping 20MHz frequency 
   *  channels will be assigned to different mesh point interfaces.
   */
  enum ChannelPolicy
  {
    SPREAD_CHANNELS,
    ZERO_CHANNEL
  };

  /**
   * \brief set the channel policy
   * \param policy the channel policy
   */
  void SetSpreadInterfaceChannels (ChannelPolicy policy);
  /**
   * \brief Set a number of interfaces in a mesh network
   * \param nInterfaces is the number of interfaces
   */
  void SetNumberOfInterfaces (uint32_t nInterfaces);

  /** 
   * \brief Install 802.11s mesh device & protocols on given node list
   * 
   * \param phyHelper           Wifi PHY helper
   * \param c               List of nodes to install
   * 
   * \return list of created mesh point devices, see MeshPointDevice
   */
  NetDeviceContainer
  Install (const WifiPhyHelper &phyHelper, NodeContainer c) const;
  /**
   * \param type the type of ns3::MeshStack.
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   */
  void SetStackInstaller (stdfwd::string type,
                          stdfwd::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                          stdfwd::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                          stdfwd::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                          stdfwd::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                          stdfwd::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                          stdfwd::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                          stdfwd::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                          stdfwd::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());

  /**
   * \brief Print statistics.
   *
   * \param device the net device
   * \param os the output stream
   */
  void Report (const ns3::Ptr<ns3::NetDevice>& device, std::ostream& os);

  /**
   * \brief Reset statistics.
   * \param device the net device
   */
  void ResetStats (const ns3::Ptr<ns3::NetDevice>& device);
  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.  The Install() method of this helper
   * should have previously been called by the user.
   *
   * \param stream first stream index to use
   * \param c NetDeviceContainer of the set of devices for which the mesh devices
   *          should be modified to use a fixed stream
   * \return the number of stream indices assigned by this helper
   */
  int64_t AssignStreams (NetDeviceContainer c, int64_t stream);

private:
  /**
   * \param phyHelper
   * \param node
   * \param channelId
   * \returns a WifiNetDevice with ready-to-use interface
   */
  Ptr<WifiNetDevice> CreateInterface (const WifiPhyHelper &phyHelper, Ptr<Node> node, uint16_t channelId) const;
  uint32_t m_nInterfaces; ///< number of interfaces
  ChannelPolicy m_spreadChannelPolicy; ///< spread channel policy
  Ptr<MeshStack> m_stack; ///< stack
  ObjectFactory m_stackFactory; ///< stack factory

  // Interface factory
  ObjectFactory m_mac; ///< the MAC
  ObjectFactory m_stationManager; ///< the station manager
  enum WifiPhyStandard m_standard; ///< phy standard

};
} // namespace ns3



