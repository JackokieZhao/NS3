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
 *          Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#include "../../core/model/log.h"
#include "../../core/model/names.h"
#include "../model/spectrum-wifi-phy.h"
#include "../model/error-rate-model.h"
#include "../model/frame-capture-model.h"
#include "../model/preamble-detection-model.h"
#include "../../mobility/model/mobility-model.h"
#include "spectrum-wifi-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SpectrumWifiHelper");

SpectrumWifiPhyHelper::SpectrumWifiPhyHelper ()
  : m_channel (0)
{
  m_phy.SetTypeId ("ns3::SpectrumWifiPhy");
}

SpectrumWifiPhyHelper
SpectrumWifiPhyHelper::Default (void)
{
  SpectrumWifiPhyHelper helper;
  helper.SetErrorRateModel ("ns3::NistErrorRateModel");
  return helper;
}

void
SpectrumWifiPhyHelper::SetChannel (Ptr<SpectrumChannel> channel)
{
  m_channel = channel;
}

void
SpectrumWifiPhyHelper::SetChannel (stdfwd::string channelName)
{
  Ptr<SpectrumChannel> channel = Names::Find<SpectrumChannel> (channelName);
  m_channel = channel;
}

Ptr<WifiPhy>
SpectrumWifiPhyHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  Ptr<SpectrumWifiPhy> phy = m_phy.Create<SpectrumWifiPhy> ();
  phy->CreateWifiSpectrumPhyInterface (device);
  Ptr<ErrorRateModel> error = m_errorRateModel.Create<ErrorRateModel> ();
  phy->SetErrorRateModel (error);
  if (m_frameCaptureModel.IsTypeIdSet ())
    {
      Ptr<FrameCaptureModel> capture = m_frameCaptureModel.Create<FrameCaptureModel> ();
      phy->SetFrameCaptureModel (capture);
    }
  if (m_preambleDetectionModel.IsTypeIdSet ())
    {
      Ptr<PreambleDetectionModel> capture = m_preambleDetectionModel.Create<PreambleDetectionModel> ();
      phy->SetPreambleDetectionModel (capture);
    }
  phy->SetChannel (m_channel);
  phy->SetDevice (device);
  phy->SetMobility (node->GetObject<MobilityModel> ());
  return phy;
}

} //namespace ns3
