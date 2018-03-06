/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * PigpioPort.h
 * This represents the input / output ports that hook into the thread.
 * Copyright (C) 2017 Florian Edelmann
 */

#ifndef PLUGINS_Pigpio_PigpioPORT_H_
#define PLUGINS_Pigpio_PigpioPORT_H_

#include <string>

#include "ola/DmxBuffer.h"
#include "olad/Port.h"
#include "olad/Preferences.h"
#include "plugins/pigpio/PigpioDevice.h"
#include "plugins/pigpio/PigpioWidget.h"
#include "plugins/pigpio/PigpioThread.h"

namespace ola {
namespace plugin {
namespace pigpio {

class PigpioInputPort : public ola::BasicInputPort {
 public:
  PigpioInputPort(PigpioDevice *parent,
                  unsigned int id,
                  PluginAdaptor *plugin_adaptor,
                  PigpioWidget *widget,
                  PigpioThread *thread)
      : BasicInputPort(parent, id, plugin_adaptor, false),
        m_widget(widget),
        m_thread(thread) {
  }
  ~PigpioInputPort() {}

  const DmxBuffer &ReadDMX() const {
    return m_thread->GetDmxInBuffer();
  }

  bool PreSetUniverse(Universe *old_universe, Universe *new_universe) {
    if (!old_universe && new_universe) {
      return m_thread->SetReceiveCallback(NewCallback(
        static_cast<BasicInputPort*>(this),
        &BasicInputPort::DmxChanged));
    }
    if (old_universe && !new_universe) {
      return m_thread->SetReceiveCallback(NULL);
    }
    return true;
  }

  std::string Description() const {
    return m_widget->Description();
  }

 private:
  PigpioWidget *m_widget;
  PigpioThread *m_thread;

  DISALLOW_COPY_AND_ASSIGN(PigpioInputPort);
};

}  // namespace pigpio
}  // namespace plugin
}  // namespace ola
#endif  // PLUGINS_Pigpio_PigpioPORT_H_
