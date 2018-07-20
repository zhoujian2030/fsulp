# Target name
LIBNAME = libcm

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

include global.mk
include filter_code.mk
include lib_rules.mk

# include
INC += -I$(INCDIR)

# compile flags
CFLAGS += -DRT_LOGGER
ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
else
CFLAGS += -UUNIT_TEST
endif


	
