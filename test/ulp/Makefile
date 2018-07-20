# Target name
TARGET = ulp

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
INC += -I$(INCDIR) $(INCGTEST) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc -I$(PROJBASE)/ulp/inc

# platform libs dependency 
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/ulp/lib/libulp.a $(PROJBASE)/cm/lib/libcm.a
else
LIBS = $(PROJBASE)/ulp/lib/libulp_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
endif
LIBS += $(LIBAGTEST)

# compile flags
CFLAGS += -UMAC_PHY_INTF_NEW -DUNIT_TEST -UISSUE_DEBUG -DPHY_DEBUG -DDPE

# link flags
LDFLAGS += -pthread

# link c or cpp target
LD := $(CXX)

