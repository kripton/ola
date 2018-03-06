# LIBRARIES
##################################################
if USE_PIGPIO
lib_LTLIBRARIES += plugins/pigpio/libolapigpio.la

# Plugin description is generated from README.md
built_sources += plugins/pigpio/PigpioPluginDescription.h
nodist_plugins_pigpio_libolapigpio_la_SOURCES = \
    plugins/pigpio/PigpioPluginDescription.h
plugins/pigpio/PigpioPluginDescription.h: plugins/pigpio/README.md plugins/pigpio/Makefile.mk plugins/convert_README_to_header.sh
	sh $(top_srcdir)/plugins/convert_README_to_header.sh $(top_srcdir)/plugins/pigpio $(top_builddir)/plugins/pigpio/PigpioPluginDescription.h

plugins_pigpio_libolapigpio_la_SOURCES = \
    plugins/pigpio/PigpioDevice.cpp \
    plugins/pigpio/PigpioDevice.h \
    plugins/pigpio/PigpioPlugin.cpp \
    plugins/pigpio/PigpioPlugin.h \
    plugins/pigpio/PigpioPort.h \
    plugins/pigpio/PigpioThread.cpp \
    plugins/pigpio/PigpioThread.h \
    plugins/pigpio/PigpioWidget.cpp \
    plugins/pigpio/PigpioWidget.h
plugins_pigpio_libolapigpio_la_LIBADD = \
    common/libolacommon.la \
    olad/plugin_api/libolaserverplugininterface.la
endif

EXTRA_DIST += plugins/pigpio/README.md
