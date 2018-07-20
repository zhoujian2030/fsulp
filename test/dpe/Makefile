# Target name
TARGET = dpe

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
INC += -I$(INCDIR) $(INCGTEST) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc -I$(PROJBASE)/dpe/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/ulp/inc

# platform libs dependency 
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/dpe/lib/libdpe.a $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a
else
LIBS = $(PROJBASE)/dpe/lib/libdpe_arm.a $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
endif
LIBS += $(LIBAGTEST)

# compile flags
CFLAGS += -Wno-write-strings -UMAC_PHY_INTF_NEW -DUNIT_TEST -UISSUE_DEBUG

# link flags
LDFLAGS += -pthread -ldl

# link c or cpp target
LD := $(CXX)
