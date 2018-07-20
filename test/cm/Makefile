# Target name
TARGET = cm

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
DEPDIR = dep
BINDIR = bin
DEPDIR = .d

include $(PROJBASE)/cm/global.mk
include $(PROJBASE)/cm/filter_code.mk
include $(PROJBASE)/cm/bin_rules.mk
include $(PROJBASE)/test/gtest_rules.mk

# include
INC += -I$(INCDIR) $(INCGTEST) -I$(PROJBASE)/cm/inc

# platform libs dependency 
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/cm/lib/libcm.a
else
LIBS = $(PROJBASE)/cm/lib/libcm_arm.a
endif
LIBS += $(LIBAGTEST)

# compile flags
CFLAGS += -Wno-write-strings -UMAC_PHY_INTF_NEW -DUNIT_TEST

# link flags
LDFLAGS += -pthread

# link c or cpp target
LD := $(CXX)

