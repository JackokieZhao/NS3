/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008,2009 INRIA, UDCAST
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * The original version of UdpClient is by  Amine Ismail
 * <amine.ismail@sophia.inria.fr> <amine.ismail@udcast.com> 
 * The rest of the code (including modifying UdpClient into
 *  EpsBearerTagUdpClient) is by Nicola Baldo <nbaldo@cttc.es> 
 */



#include "../../core/model/simulator.h"
#include "../../core/model/log.h"
#include "../../core/model/test.h"
#include "../helper/point-to-point-epc-helper.h"
#include "../model/epc-enb-application.h"
#include "../../applications/helper/packet-sink-helper.h"
#include "../../point-to-point/helper/point-to-point-helper.h"
#include "../../csma/helper/csma-helper.h"
#include "../../internet/helper/internet-stack-helper.h"
#include "../../internet/helper/ipv4-address-helper.h"
#include "../../network/utils/inet-socket-address.h"
#include "../../applications/model/packet-sink.h"
#include "../../internet/helper/ipv4-static-routing-helper.h"
#include "../../internet/model/ipv4-static-routing.h"
#include "../../internet/model/ipv4-interface.h"
#include "../../network/utils/mac48-address.h"
#include "../../applications/model/seq-ts-header.h"
#include "../model/eps-bearer-tag.h"
#include "../../internet/model/arp-cache.h"
#include "../../core/model/boolean.h"
#include "../../core/model/uinteger.h"
#include "../../core/model/config.h"
#include "lte-test-entities.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("EpcTestS1uUplink");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * A Udp client. Sends UDP packet carrying sequence number and time
 * stamp but also including the EpsBearerTag. This tag is normally
 * generated by the LteEnbNetDevice when forwarding packet in the
 * uplink. But in this test we don't have the LteEnbNetDevice, because
 * we test the S1-U interface with simpler devices to make sure it
 * just works.
 * 
 */
class EpsBearerTagUdpClient : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId
  GetTypeId (void);

  EpsBearerTagUdpClient ();
  /**
   * Constructor
   * 
   * \param rnti the RNTI
   * \param bid the BID
   */
  EpsBearerTagUdpClient (uint16_t rnti, uint8_t bid);

  virtual ~EpsBearerTagUdpClient ();

  /**
   * \brief set the remote address and port
   * \param ip remote IP address
   * \param port remote port
   */
  void SetRemote (Ipv4Address ip, uint16_t port);

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Schedule transmit function
   * \param dt the delta time
   */
  void ScheduleTransmit (Time dt);
  /// Send function
  void Send (void);

  uint32_t m_count; ///< maximum number of packets to send
  Time m_interval; ///< the time between packets
  uint32_t m_size; ///< the size of packets generated
  
  uint32_t m_sent; ///< number of packets sent
  Ptr<Socket> m_socket; ///< the socket 
  Ipv4Address m_peerAddress; ///< the peer address of the outbound packets
  uint16_t m_peerPort; ///< the destination port of the outbound packets
  EventId m_sendEvent; ///< the send event

  uint16_t m_rnti; ///< the RNTI
  uint8_t m_bid; ///< the bearer identificator

};



TypeId
EpsBearerTagUdpClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpsBearerTagUdpClient")
    .SetParent<Application> ()
    .AddConstructor<EpsBearerTagUdpClient> ()
    .AddAttribute ("MaxPackets",
                   "The maximum number of packets the application will send",
                   UintegerValue (100),
                   MakeUintegerAccessor (&EpsBearerTagUdpClient::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between packets", TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&EpsBearerTagUdpClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute (
      "RemoteAddress",
      "The destination Ipv4Address of the outbound packets",
      Ipv4AddressValue (),
      MakeIpv4AddressAccessor (&EpsBearerTagUdpClient::m_peerAddress),
      MakeIpv4AddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&EpsBearerTagUdpClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 12 bytes which is the size of the header carrying the sequence number and the time stamp.",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&EpsBearerTagUdpClient::m_size),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

EpsBearerTagUdpClient::EpsBearerTagUdpClient ()
  : m_rnti (0),
    m_bid (0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
}

EpsBearerTagUdpClient::EpsBearerTagUdpClient (uint16_t rnti, uint8_t bid)
  : m_rnti (rnti),
    m_bid (bid)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
}

EpsBearerTagUdpClient::~EpsBearerTagUdpClient ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
EpsBearerTagUdpClient::SetRemote (Ipv4Address ip, uint16_t port)
{
  m_peerAddress = ip;
  m_peerPort = port;
}

void
EpsBearerTagUdpClient::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void
EpsBearerTagUdpClient::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      m_socket->Bind ();
      m_socket->Connect (InetSocketAddress (m_peerAddress, m_peerPort));
    }

  m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &EpsBearerTagUdpClient::Send, this);
}

void
EpsBearerTagUdpClient::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}

void
EpsBearerTagUdpClient::Send (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_sendEvent.IsExpired ());
  SeqTsHeader seqTs;
  seqTs.SetSeq (m_sent);
  Ptr<Packet> p = Create<Packet> (m_size-(8+4)); // 8+4 : the size of the seqTs header
  p->AddHeader (seqTs);

  EpsBearerTag tag (m_rnti, m_bid);
  p->AddPacketTag (tag);

  if ((m_socket->Send (p)) >= 0)
    {
      ++m_sent;
      NS_LOG_INFO ("TraceDelay TX " << m_size << " bytes to "
                                    << m_peerAddress << " Uid: " << p->GetUid ()
                                    << " Time: " << (Simulator::Now ()).GetSeconds ());

    }
  else
    {
      NS_LOG_INFO ("Error while sending " << m_size << " bytes to "
                                          << m_peerAddress);
    }

  if (m_sent < m_count)
    {
      m_sendEvent = Simulator::Schedule (m_interval, &EpsBearerTagUdpClient::Send, this);
    }
}



/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Custom test structure to hold information of data transmitted in the uplink per UE
 */
struct UeUlTestData
{
  /**
   * Constructor
   *
   * \param n number of packets
   * \param s packet size
   * \param r the RNTI
   * \param l the BID
   */
  UeUlTestData (uint32_t n, uint32_t s, uint16_t r, uint8_t l);

  uint32_t numPkts; ///< the number of packets sent
  uint32_t pktSize; ///< the packet size
  uint16_t rnti; ///< the RNTI
  uint8_t bid; ///< the BID
 
  Ptr<PacketSink> serverApp; ///< the server application
  Ptr<Application> clientApp; ///< the client application
};

  UeUlTestData::UeUlTestData (uint32_t n, uint32_t s, uint16_t r, uint8_t l)
  : numPkts (n),
    pktSize (s),
    rnti (r),
    bid (l)
{
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Custom structure containing information about data sent in the uplink 
 * of eNodeB. Includes the information of the data sent in the uplink per UE.
 */
struct EnbUlTestData
{
  std::vector<UeUlTestData> ues; ///< the list of UEs
};


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief EpcS1uUlTestCase class
 */
class EpcS1uUlTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param name the reference name
   * \param v the list of UE lists
   */
  EpcS1uUlTestCase (stdfwd::string name, std::vector<EnbUlTestData> v);
  virtual ~EpcS1uUlTestCase ();

private:
  virtual void DoRun (void);
  std::vector<EnbUlTestData> m_enbUlTestData; ///< ENB UL test data
};


EpcS1uUlTestCase::EpcS1uUlTestCase (stdfwd::string name, std::vector<EnbUlTestData> v)
  : TestCase (name),
    m_enbUlTestData (v)
{
}

EpcS1uUlTestCase::~EpcS1uUlTestCase ()
{
}

void 
EpcS1uUlTestCase::DoRun ()
{
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // allow jumbo packets
  Config::SetDefault ("ns3::CsmaNetDevice::Mtu", UintegerValue (30000));
  Config::SetDefault ("ns3::PointToPointNetDevice::Mtu", UintegerValue (30000));
  epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue (30000));
  
  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate",  DataRateValue (DataRate ("100Gb/s")));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);  
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetNodesIpIfaceContainer = ipv4h.Assign (internetDevices);
  
  // setup default gateway for the remote hosts
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());

  // hardcoded UE addresses for now
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.255.255.0"), 1);
  


  uint16_t udpSinkPort = 1234;
    
  NodeContainer enbs;
  uint16_t cellIdCounter = 0;
  uint64_t imsiCounter = 0;

  for (std::vector<EnbUlTestData>::iterator enbit = m_enbUlTestData.begin ();
       enbit < m_enbUlTestData.end ();
       ++enbit)
    {
      Ptr<Node> enb = CreateObject<Node> ();
      enbs.Add (enb);      

      // we test EPC without LTE, hence we use:
      // 1) a CSMA network to simulate the cell
      // 2) a raw socket opened on the CSMA device to simulate the LTE socket

      uint16_t cellId = ++cellIdCounter;

      NodeContainer ues;
      ues.Create (enbit->ues.size ());

      NodeContainer cell;
      cell.Add (ues);
      cell.Add (enb);

      CsmaHelper csmaCell;      
      NetDeviceContainer cellDevices = csmaCell.Install (cell);

      // the eNB's CSMA NetDevice acting as an LTE NetDevice. 
      Ptr<NetDevice> enbDevice = cellDevices.Get (cellDevices.GetN () - 1);

      // Note that the EpcEnbApplication won't care of the actual NetDevice type
      epcHelper->AddEnb (enb, enbDevice, cellId);      
      
       // Plug test RRC entity
      Ptr<EpcEnbApplication> enbApp = enb->GetApplication (0)->GetObject<EpcEnbApplication> ();
      NS_ASSERT_MSG (enbApp != 0, "cannot retrieve EpcEnbApplication");
      Ptr<EpcTestRrc> rrc = CreateObject<EpcTestRrc> ();
      enb->AggregateObject (rrc);
      rrc->SetS1SapProvider (enbApp->GetS1SapProvider ());
      enbApp->SetS1SapUser (rrc->GetS1SapUser ());
      
      // we install the IP stack on UEs only
      InternetStackHelper internet;
      internet.Install (ues);

      // assign IP address to UEs, and install applications
      for (uint32_t u = 0; u < ues.GetN (); ++u)
        {
          Ptr<NetDevice> ueLteDevice = cellDevices.Get (u);
          Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevice));

          Ptr<Node> ue = ues.Get (u);

          // disable IP Forwarding on the UE. This is because we use
          // CSMA broadcast MAC addresses for this test. The problem
          // won't happen with a LteUeNetDevice. 
          Ptr<Ipv4> ueIpv4 = ue->GetObject<Ipv4> ();
          ueIpv4->SetAttribute ("IpForward", BooleanValue (false));

          // tell the UE to route all packets to the GW
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueIpv4);
          Ipv4Address gwAddr = epcHelper->GetUeDefaultGatewayAddress ();
          NS_LOG_INFO ("GW address: " << gwAddr);
          ueStaticRouting->SetDefaultRoute (gwAddr, 1);

          // since the UEs in this test use CSMA with IP enabled, and
          // the eNB uses CSMA but without IP, we fool the UE's ARP
          // cache into thinking that the IP address of the GW can be
          // reached by sending a CSMA packet to the broadcast
          // address, so the eNB will get it.       
          int32_t ueLteIpv4IfIndex = ueIpv4->GetInterfaceForDevice (ueLteDevice);
          Ptr<Ipv4L3Protocol> ueIpv4L3Protocol = ue->GetObject<Ipv4L3Protocol> ();
          Ptr<Ipv4Interface> ueLteIpv4Iface = ueIpv4L3Protocol->GetInterface (ueLteIpv4IfIndex);
          Ptr<ArpCache> ueArpCache = ueLteIpv4Iface->GetArpCache (); 
          ueArpCache->SetAliveTimeout (Seconds (1000));          
          ArpCache::Entry* arpCacheEntry = ueArpCache->Add (gwAddr);
          arpCacheEntry->SetMacAddress (Mac48Address::GetBroadcast ());
          arpCacheEntry->MarkPermanent ();
  
          
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", 
                                             InetSocketAddress (Ipv4Address::GetAny (), udpSinkPort));          
          ApplicationContainer sinkApp = packetSinkHelper.Install (remoteHost);
          sinkApp.Start (Seconds (1.0));
          sinkApp.Stop (Seconds (10.0));
          enbit->ues[u].serverApp = sinkApp.Get (0)->GetObject<PacketSink> ();
          
          Time interPacketInterval = Seconds (0.01);
          Ptr<EpsBearerTagUdpClient> client = CreateObject<EpsBearerTagUdpClient> (enbit->ues[u].rnti, enbit->ues[u].bid);
          client->SetAttribute ("RemoteAddress", Ipv4AddressValue (internetNodesIpIfaceContainer.GetAddress (1)));
          client->SetAttribute ("RemotePort", UintegerValue (udpSinkPort));          
          client->SetAttribute ("MaxPackets", UintegerValue (enbit->ues[u].numPkts));
          client->SetAttribute ("Interval", TimeValue (interPacketInterval));
          client->SetAttribute ("PacketSize", UintegerValue (enbit->ues[u].pktSize));
          ue->AddApplication (client);
          ApplicationContainer clientApp;
          clientApp.Add (client);
          clientApp.Start (Seconds (2.0));
          clientApp.Stop (Seconds (10.0));   
          enbit->ues[u].clientApp = client;

          uint64_t imsi = ++imsiCounter;
          epcHelper->AddUe (ueLteDevice, imsi);
          epcHelper->ActivateEpsBearer (ueLteDevice, imsi, EpcTft::Default (), EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
          Simulator::Schedule (MilliSeconds (10),
                               &EpcEnbS1SapProvider::InitialUeMessage,
                               enbApp->GetS1SapProvider (), imsi, enbit->ues[u].rnti);
          // need this since all sinks are installed in the same node
          ++udpSinkPort; 
        } 
            
    } 
  
  Simulator::Run ();

  for (std::vector<EnbUlTestData>::iterator enbit = m_enbUlTestData.begin ();
       enbit < m_enbUlTestData.end ();
       ++enbit)
    {
      for (std::vector<UeUlTestData>::iterator ueit = enbit->ues.begin ();
           ueit < enbit->ues.end ();
           ++ueit)
        {
          NS_TEST_ASSERT_MSG_EQ (ueit->serverApp->GetTotalRx (), (ueit->numPkts) * (ueit->pktSize), "wrong total received bytes");
        }      
    }
  
  Simulator::Destroy ();
}





/**
 * Test that the S1-U interface implementation works correctly
 */
class EpcS1uUlTestSuite : public TestSuite
{
public:
  EpcS1uUlTestSuite ();
  
} g_epcS1uUlTestSuiteInstance;

EpcS1uUlTestSuite::EpcS1uUlTestSuite ()
  : TestSuite ("epc-s1u-uplink", SYSTEM)
{  
  std::vector<EnbUlTestData> v1;  
  EnbUlTestData e1;
  UeUlTestData f1 (1, 100, 1, 1);
  e1.ues.push_back (f1);
  v1.push_back (e1);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 1UE", v1), TestCase::QUICK);


  std::vector<EnbUlTestData> v2;  
  EnbUlTestData e2;
  UeUlTestData f2_1 (1, 100, 1, 1);
  e2.ues.push_back (f2_1);
  UeUlTestData f2_2 (2, 200, 2, 1);
  e2.ues.push_back (f2_2);
  v2.push_back (e2);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 2UEs", v2), TestCase::QUICK);


  std::vector<EnbUlTestData> v3;  
  v3.push_back (e1);
  v3.push_back (e2);
  AddTestCase (new EpcS1uUlTestCase ("2 eNBs", v3), TestCase::QUICK);


  EnbUlTestData e3;
  UeUlTestData f3_1 (3, 50, 1, 1);
  e3.ues.push_back (f3_1);
  UeUlTestData f3_2 (5, 1472, 2, 1);
  e3.ues.push_back (f3_2);
  UeUlTestData f3_3 (1, 1, 3, 1);
  e3.ues.push_back (f3_2);
  std::vector<EnbUlTestData> v4;  
  v4.push_back (e3);
  v4.push_back (e1);
  v4.push_back (e2);
  AddTestCase (new EpcS1uUlTestCase ("3 eNBs", v4), TestCase::QUICK);

  std::vector<EnbUlTestData> v5;  
  EnbUlTestData e5;
  UeUlTestData f5 (10, 3000, 1, 1);
  e5.ues.push_back (f5);
  v5.push_back (e5);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 10 pkts 3000 bytes each", v5), TestCase::QUICK);

  std::vector<EnbUlTestData> v6;  
  EnbUlTestData e6;
  UeUlTestData f6 (50, 3000, 1, 1);
  e6.ues.push_back (f6);
  v6.push_back (e6);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 50 pkts 3000 bytes each", v6), TestCase::QUICK);

  std::vector<EnbUlTestData> v7;  
  EnbUlTestData e7;
  UeUlTestData f7 (10, 15000, 1, 1);
  e7.ues.push_back (f7);
  v7.push_back (e7);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 10 pkts 15000 bytes each", v7), TestCase::QUICK);

  std::vector<EnbUlTestData> v8;  
  EnbUlTestData e8;
  UeUlTestData f8 (100, 15000, 1, 1);
  e8.ues.push_back (f8);
  v8.push_back (e8);
  AddTestCase (new EpcS1uUlTestCase ("1 eNB, 100 pkts 15000 bytes each", v8), TestCase::QUICK);
  
}
