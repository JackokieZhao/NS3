/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 INRIA, 2012 University of Washington
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
 */

#pragma once

#include "../../../3rd-party/cpp-std-fwd/stdfwd.h"

#include "../../core/model/attribute.h"
#include "../model/fd-net-device.h"
#include "fd-net-device-helper.h"
#include "../../core/model/object-factory.h"
#include "../../network/helper/net-device-container.h"
#include "../../network/helper/node-container.h"

namespace ns3 {

/**
 * \ingroup fd-net-device
 * \brief build a set of FdNetDevice objects attached to a physical network
 * interface
 *
 */
class EmuFdNetDeviceHelper : public FdNetDeviceHelper
{
public:
  /**
   * Construct a EmuFdNetDeviceHelper.
   */
  EmuFdNetDeviceHelper ();
  virtual ~EmuFdNetDeviceHelper ()
  {
  }

  /**
   * Get the device name of this device.
   *
   * \returns The device name of this device.
   */
  stdfwd::string GetDeviceName (void);

  /**
   * Set the device name of this device.
   *
   * \param deviceName The device name of this device.
   */
  void SetDeviceName (stdfwd::string deviceName);

protected:

  /**
   * This method creates an ns3::FdNetDevice attached to a physical network
   * interface
   *
   * \param node The node to install the device in
   * \returns A container holding the added net device.
   */
  Ptr<NetDevice> InstallPriv (Ptr<Node> node) const;

  /**
   * Sets a file descriptor on the FileDescriptorNetDevice.
   */
  virtual void SetFileDescriptor (Ptr<FdNetDevice> device) const;

  /**
   * Call out to a separate process running as suid root in order to get a raw
   * socket.  We do this to avoid having the entire simulation running as root.
   * \return the rawSocket number
   */
  virtual int CreateFileDescriptor (void) const;

  /**
   * The unix/linux name of the underlying device (e.g., eth0)
   */
  stdfwd::string m_deviceName;
};

} // namespace ns3


