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
 * PigpioDevice.cpp
 * This represents a device and manages thread, widget and input/output ports.
 * Copyright (C) 2017 Florian Edelmann
 */

#include <string>
#include <memory>
#include "ola/Logging.h"
#include "ola/StringUtils.h"
#include "plugins/pigpio/PigpioDevice.h"
#include "plugins/pigpio/PigpioPort.h"

namespace ola {
namespace plugin {
namespace pigpio {

using std::string;

const unsigned int PigpioDevice::PREF_BLOCKLENGTH_DEFAULT = 4096;
const char PigpioDevice::PREF_BLOCKLENGTH_KEY[] = "-blocklength";


PigpioDevice::PigpioDevice(AbstractPlugin *owner,
                           class Preferences *preferences,
                           PluginAdaptor *plugin_adaptor,
                           const string &name,
                           const string &path)
    : Device(owner, name),
      m_preferences(preferences),
      m_plugin_adaptor(plugin_adaptor),
      m_name(name),
      m_path(path) {
  // set up some per-device default configuration if not already set
  SetDefaults();

  // now read per-device configuration
  if (!StringToInt(m_preferences->GetValue(DeviceBlocklength()),
                                           &m_blocklength)) {
    m_blocklength = PREF_BLOCKLENGTH_DEFAULT;
  }

  m_widget.reset(new PigpioWidget(path));
  m_thread.reset(new PigpioThread(m_widget.get(), m_blocklength));
}

PigpioDevice::~PigpioDevice() {
  if (m_widget->IsOpen()) {
    m_widget->Close();
  }
  m_thread->Stop();
}

bool PigpioDevice::StartHook() {
  AddPort(new PigpioInputPort(this, 0, m_plugin_adaptor, m_widget.get(),
                              m_thread.get()));
  return true;
}

string PigpioDevice::DeviceBlocklength() const {
  return m_path + PREF_BLOCKLENGTH_KEY;
}

/**
 * Set the default preferences for this one Device
 */
void PigpioDevice::SetDefaults() {
  if (!m_preferences) {
    return;
  }

  bool save = m_preferences->SetDefaultValue(DeviceBlocklength(),
                                             UIntValidator(1, 65535),
                                             PREF_BLOCKLENGTH_DEFAULT);
  if (save) {
    m_preferences->Save();
  }
}

}  // namespace pigpio
}  // namespace plugin
}  // namespace ola
