headerdir=$(includedir)/RakNet/RakVoice
INCLUDES=-I..
# Do we build RakVoice 
if BUILD_RAKVOICE
header_HEADERS=\
	RakVoice.h\
	RakVoiceFactory.h\
	RakVoiceInterface.h

libRakNet_voice_la_SOURCES=\
	RakVoice.cpp\
	RakVoiceFactory.cpp

# Do we build it whithin RakNet ? 
if BUILD_MONOLITHIC 
noinst_LTLIBRARIES=libRakNet_voice.la
else
# Build it as a special library ?
lib_LTLIBRARIES=libRakNet_voice.la
endif

endif
