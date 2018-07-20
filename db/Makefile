# Target name
LIBNAME = libdb

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

include $(PROJBASE)/cm/global.mk
include $(PROJBASE)/cm/filter_code.mk
include $(PROJBASE)/cm/lib_rules.mk

# include 
INC += -I$(INCDIR) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc

# compile flags
#SQLITE_THREADSAFE: 0, 1, 2
#Multi-Threaded mode
CFLAGS = -Wall -g -DOS_LINUX -DSQLITE_THREADSAFE=2
ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
else
CFLAGS += -UUNIT_TEST
endif

