# Target name
TARGET = deamon

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
INC += -I$(INCDIR)

# compile flags
CFLAGS += -UMAC_PHY_INTF_NEW -UUNIT_TEST

# link flags
LDFLAGS += -pthread
	
# link c or cpp target
LD := $(CXX)
