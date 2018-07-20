# Target name
TARGET = dpe

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
INC += -I$(INCDIR) -I$(PROJBASE)/cm/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/dpe/inc -I$(PROJBASE)/ulp/inc

# platform libs dependency 
ifeq ($(PLATFORM), arm)
LIBS += $(PROJBASE)/dpe/lib/libdpe_arm.a $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a 
else
ifeq ($(PLATFORM), ppc)
LIBS += $(PROJBASE)/dpe/lib/libdpe_ppc.a $(PROJBASE)/db/lib/libdb_ppc.a $(PROJBASE)/cm/lib/libcm_ppc.a 
else
LIBS += $(PROJBASE)/dpe/lib/libdpe.a $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a
endif
endif

# compile flags
CFLAGS += -UUNIT_TEST

# link flags
LDFLAGS += -pthread -ldl

# link c or cpp target
LD := $(CXX)
	
