/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * uan-helper.cc
 *
 *  Copyright (c) 2008 University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 9-Oct-2008
 *  Author: Leonard Tracy <lentracy@u.washington.edu>
 *
 *
 */

#include "uan-helper.h"
#include "../model/uan-net-device.h"
#include "../model/uan-mac.h"
#include "../model/uan-phy.h"
#include "../model/uan-channel.h"
#include "../model/uan-prop-model.h"
#include "../model/uan-prop-model-ideal.h"
#include "../model/uan-transducer.h"
#include "../../mobility/model/mobility-model.h"
#include "../../core/model/log.h"
#include "../model/uan-tx-mode.h"
#include "../../core/model/config.h"
#include "../../core/model/simulator.h"
#include "../model/uan-noise-model-default.h"

#include <sstream>
#include "../../../3rd-party/cpp-std-fwd/stdfwd.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UanHelper");

/**
 * Ascii trace callback on Phy transmit events.
 *
 * \param os The output stream.
 * \param context The node and device ids.
 * \param packet The transmitted packet.
 * \param txPowerDb The transmission power.
 * \param mode The transmission mode.
 */
static void AsciiPhyTxEvent (std::ostream *os, stdfwd::string context,
                             Ptr<const Packet> packet, double txPowerDb, UanTxMode mode)
{
  NS_UNUSED (txPowerDb);
  *os << "+ " << Simulator::Now ().GetSeconds () << " " << context << " " << *packet << std::endl;
}

/**
 * Ascii trace callback on successful packet reception.
 *
 * \param os The output stream.
 * \param context The node and device ids.
 * \param packet The received packet.
 * \param snr The received signal to noise ratio.
 * \param mode The channel transmission mode.
 */
static void AsciiPhyRxOkEvent (std::ostream *os, stdfwd::string context,
                               Ptr<const Packet> packet, double snr, UanTxMode mode)
{
  NS_UNUSED (snr);
  *os << "r " << Simulator::Now ().GetSeconds () << " " << context << " " << *packet << std::endl;
}

UanHelper::UanHelper ()
{
  m_mac.SetTypeId ("ns3::UanMacAloha");
  m_phy.SetTypeId ("ns3::UanPhyGen");
  m_transducer.SetTypeId ("ns3::UanTransducerHd");

}

UanHelper::~UanHelper ()
{

}


void
UanHelper::SetMac (stdfwd::string macType,
                   stdfwd::string n0, const AttributeValue &v0,
                   stdfwd::string n1, const AttributeValue &v1,
                   stdfwd::string n2, const AttributeValue &v2,
                   stdfwd::string n3, const AttributeValue &v3,
                   stdfwd::string n4, const AttributeValue &v4,
                   stdfwd::string n5, const AttributeValue &v5,
                   stdfwd::string n6, const AttributeValue &v6,
                   stdfwd::string n7, const AttributeValue &v7)
{
  m_mac = ObjectFactory ();
  m_mac.SetTypeId (macType);
  m_mac.Set (n0, v0);
  m_mac.Set (n1, v1);
  m_mac.Set (n2, v2);
  m_mac.Set (n3, v3);
  m_mac.Set (n4, v4);
  m_mac.Set (n5, v5);
  m_mac.Set (n6, v6);
  m_mac.Set (n7, v7);
}

void
UanHelper::SetPhy (stdfwd::string phyType,
                   stdfwd::string n0, const AttributeValue &v0,
                   stdfwd::string n1, const AttributeValue &v1,
                   stdfwd::string n2, const AttributeValue &v2,
                   stdfwd::string n3, const AttributeValue &v3,
                   stdfwd::string n4, const AttributeValue &v4,
                   stdfwd::string n5, const AttributeValue &v5,
                   stdfwd::string n6, const AttributeValue &v6,
                   stdfwd::string n7, const AttributeValue &v7)
{
  m_phy = ObjectFactory ();
  m_phy.SetTypeId (phyType);
  m_phy.Set (n0, v0);
  m_phy.Set (n1, v1);
  m_phy.Set (n2, v2);
  m_phy.Set (n3, v3);
  m_phy.Set (n4, v4);
  m_phy.Set (n5, v5);
  m_phy.Set (n6, v6);
  m_phy.Set (n7, v7);

}

void
UanHelper::SetTransducer (stdfwd::string type,
                          stdfwd::string n0, const AttributeValue &v0,
                          stdfwd::string n1, const AttributeValue &v1,
                          stdfwd::string n2, const AttributeValue &v2,
                          stdfwd::string n3, const AttributeValue &v3,
                          stdfwd::string n4, const AttributeValue &v4,
                          stdfwd::string n5, const AttributeValue &v5,
                          stdfwd::string n6, const AttributeValue &v6,
                          stdfwd::string n7, const AttributeValue &v7)
{
  m_transducer = ObjectFactory ();
  m_transducer.SetTypeId (type);
  m_transducer.Set (n0, v0);
  m_transducer.Set (n1, v1);
  m_transducer.Set (n2, v2);
  m_transducer.Set (n3, v3);
  m_transducer.Set (n4, v4);
  m_transducer.Set (n5, v5);
  m_transducer.Set (n6, v6);
  m_transducer.Set (n7, v7);

}

void
UanHelper::EnableAscii (std::ostream &os, uint32_t nodeid, uint32_t deviceid)
{
  Packet::EnablePrinting ();
  std::ostringstream oss;

  oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::UanNetDevice/Phy/RxOk";
  Config::Connect (oss.str (), MakeBoundCallback (&AsciiPhyRxOkEvent, &os));

  oss.str ("");

  oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::UanNetDevice/Phy/Tx";
  Config::Connect (oss.str (), MakeBoundCallback (&AsciiPhyTxEvent, &os));
}

void
UanHelper::EnableAscii (std::ostream &os, NetDeviceContainer d)
{
  for (NetDeviceContainer::Iterator i = d.Begin (); i != d.End (); ++i)
    {
      Ptr<NetDevice> dev = *i;
      EnableAscii (os, dev->GetNode ()->GetId (), dev->GetIfIndex ());
    }
}

void
UanHelper::EnableAscii (std::ostream &os, NodeContainer n)
{
  NetDeviceContainer devs;
  for (NodeContainer::Iterator i = n.Begin (); i != n.End (); ++i)
    {
      Ptr<Node> node = *i;
      for (uint32_t j = 0; j < node->GetNDevices (); ++j)
        {
          devs.Add (node->GetDevice (j));
        }
    }
  EnableAscii (os, devs);
}

void
UanHelper::EnableAsciiAll (std::ostream &os)
{
  EnableAscii (os, NodeContainer::GetGlobal ());
}

NetDeviceContainer
UanHelper::Install (NodeContainer c) const
{
  Ptr<UanChannel> channel = CreateObject<UanChannel> ();
  Ptr<UanNoiseModelDefault> noise = CreateObject<UanNoiseModelDefault> ();
  channel->SetPropagationModel (CreateObject<UanPropModelIdeal> ());
  channel->SetNoiseModel (noise);

  return Install (c, channel);
}

NetDeviceContainer
UanHelper::Install (NodeContainer c, Ptr<UanChannel> channel) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      Ptr<Node> node = *i;

      Ptr<UanNetDevice> device = Install (node, channel);

      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ());
    }
  return devices;
}

Ptr<UanNetDevice>
UanHelper::Install (Ptr<Node> node, Ptr<UanChannel> channel) const
{
  Ptr<UanNetDevice> device = CreateObject<UanNetDevice> ();

  Ptr<UanMac> mac = m_mac.Create<UanMac> ();
  Ptr<UanPhy> phy = m_phy.Create<UanPhy> ();
  Ptr<UanTransducer> trans = m_transducer.Create<UanTransducer> ();

  mac->SetAddress (Mac8Address::Allocate ());
  device->SetMac (mac);
  device->SetPhy (phy);
  device->SetTransducer (trans);
  device->SetChannel (channel);

  node->AddDevice (device);

  return device;
}

int64_t
UanHelper::AssignStreams (NetDeviceContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  Ptr<NetDevice> netDevice;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      netDevice = (*i);
      Ptr<UanNetDevice> uan = DynamicCast<UanNetDevice> (netDevice);
      if (uan)
        {
          currentStream += uan->GetPhy ()->AssignStreams (currentStream);
          currentStream += uan->GetMac ()->AssignStreams (currentStream);
        }
    }
  return (currentStream - stream);
}

} // end namespace ns3
