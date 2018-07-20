# Target name
TARGET = db

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
INC += -I$(INCDIR) $(INCGTEST) -I$(PROJBASE)/db/inc

# platform libs dependency 
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a
else
LIBS = $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
endif
LIBS += $(LIBAGTEST)

# compile flags
CFLAGS += -DUNIT_TEST

# link flags
LDFLAGS += -pthread -ldl

# link c or cpp target
LD := $(CXX)

