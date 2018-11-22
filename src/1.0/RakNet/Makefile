include makefile.defs

all: static shared voicestatic voiceshared

static:
	make -C Source "BASE_DIR=$(PWD)" static

shared:
	make -C Source "BASE_DIR=$(PWD)" shared

voicestatic:
	make -C Source/RakVoice "BASE_DIR=$(PWD)" voicestatic

voiceshared:
	make -C Source/RakVoice "BASE_DIR=$(PWD)" voiceshared

clean:
	make -C Source "BASE_DIR=$(PWD)" clean
	make -C Source/RakVoice "BASE_DIR=$(PWD)" clean

install:
	rm -f $(LIBS_DIR)/libraknet.so.$(VERSION)
	rm -f $(LIBS_DIR)/libraknet.so
	rm -f $(LIBS_DIR)/librakvoice.so.$(VERSION)
	rm -f $(LIBS_DIR)/librakvoice.so
	cp $(PWD)/Lib/linux/libraknet.so.$(VERSION) $(LIBS_DIR)/
	ln -s $(LIBS_DIR)/libraknet.so.$(VERSION) $(LIBS_DIR)/libraknet.so
	cp $(PWD)/Lib/linux/librakvoice.so.$(VERSION) $(LIBS_DIR)/
	ln -s $(LIBS_DIR)/librakvoice.so.$(VERSION) $(LIBS_DIR)/librakvoice.so
	mkdir -p $(INCLUDE_DIR)/raknet
	cp -r $(PWD)/Include/* $(INCLUDE_DIR)/raknet
#   mkdir $(INCLUDE_DIR)/RakVoice;
#   cp $(PWD)/include/RakVoice/* $(INCLUDE_DIR)/RakVoice;
#   mkdir $(INCLUDE_DIR)/Autopatcher;
#   cp $(PWD)/include/Autopatcher/* $(INCLUDE_DIR)/Autopatcher;


uninstall:
	rm -f $(LIBS_DIR)/libraknet.so.$(VERSION)
	rm -f $(LIBS_DIR)/libraknet.so
	rm -f $(LIBS_DIR)/librakvoice.so.$(VERSION)
	rm -f $(LIBS_DIR)/librakvoice.so
	rm -rf $(INCLUDE_DIR)/raknet 
