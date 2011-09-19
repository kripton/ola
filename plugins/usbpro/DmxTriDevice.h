/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * DmxTriDevice.h
 * The Jese DMX-TRI device.
 * Copyright (C) 2010 Simon Newton
 */

#ifndef PLUGINS_USBPRO_DMXTRIDEVICE_H_
#define PLUGINS_USBPRO_DMXTRIDEVICE_H_

#include <string>
#include "ola/DmxBuffer.h"
#include "plugins/usbpro/DmxTriWidget.h"
#include "plugins/usbpro/UsbDevice.h"

namespace ola {
namespace plugin {
namespace usbpro {


class DmxTriWidget;


/*
 * An DMX TRI Device
 */
class DmxTriDevice: public UsbDevice {
  public:
    DmxTriDevice(ola::AbstractPlugin *owner,
                 const string &name,
                 DmxTriWidget *widget,
                 uint16_t esta_id,
                 uint16_t device_id,
                 uint32_t serial);
    ~DmxTriDevice() {}

    string DeviceId() const { return m_device_id; }
    bool StartHook();
    void PrePortStop();

  private:
    string m_device_id;
    DmxTriWidget *m_tri_widget;
};


/*
 * A single output port per device
 */
class DmxTriOutputPort: public BasicOutputPort {
  public:
    DmxTriOutputPort(DmxTriDevice *parent,
                     DmxTriWidget *widget);
    ~DmxTriOutputPort();

    bool WriteDMX(const DmxBuffer &buffer, uint8_t priority);
    string Description() const { return ""; }

    void HandleRDMRequest(const ola::rdm::RDMRequest *request,
                          ola::rdm::RDMCallback *callback);

    void PostSetUniverse(Universe *old_universe, Universe *new_universe) {
      if (new_universe)
        m_tri_widget->SendUIDUpdate();
      (void) old_universe;
    }

    void RunRDMDiscovery();
    void RunIncrementalDiscovery();

  private:
    DmxTriDevice *m_device;
    DmxTriWidget *m_tri_widget;
};
}  // usbpro
}  // plugin
}  // ola
#endif  // PLUGINS_USBPRO_DMXTRIDEVICE_H_
