# Target name
TARGET = ulp

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

include $(PROJBASE)/cm/global.mk
include $(PROJBASE)/cm/filter_code.mk
include $(PROJBASE)/cm/bin_rules.mk

# include
INC += -I$(INCDIR) -I$(PROJBASE)/cm/inc -I$(PROJBASE)/ulp/inc

# platform dependency
ifeq ($(PLATFORM), arm)
LIBS += $(PROJBASE)/ulp/lib/libulp_arm.a $(PROJBASE)/cm/lib/libcm_arm.a $(PROJBASE)/cm/extlib/libnetcp.a
else
ifeq ($(PLATFORM), ppc)
LIBS += $(PROJBASE)/ulp/lib/libulp_ppc.a $(PROJBASE)/cm/lib/libcm_ppc.a 
else
LIBS += $(PROJBASE)/ulp/lib/libulp.a $(PROJBASE)/cm/lib/libcm.a
endif
endif

# compile flags
CFLAGS += -UMAC_PHY_INTF_NEW -UUNIT_TEST

# link flags
LDFLAGS += -pthread

# link c or cpp target
LD := $(CC)

	