# Target name
LIBNAME = libulp

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

include $(PROJBASE)/cm/global.mk
include $(PROJBASE)/cm/filter_code.mk
include $(PROJBASE)/cm/lib_rules.mk

# Libraries
INC += -I$(INCDIR) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc

CFLAGS += -UMAC_PHY_INTF_NEW -UISSUE_DEBUG -DDPE -DHEARTBEAT_DEBUG -DPHY_DEBUG

ifeq ($(PLATFORM), arm)
ifeq ($(BUILD_TARGET), nodpe)
CFLAGS += -DENDIAN_CONVERT
endif
endif

ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
else
CFLAGS += -UUNIT_TEST
endif

