# Target  name
LIBA = libdb.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib

# Libraries
INC += -I$(PROJBASE)/cm/include -I$(PROJBASE)/cm/inc
LIBS =

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.c=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include ../cm/global.mk

#SQLITE_THREADSAFE: 0, 1, 2
#Multi-Threaded mode
CFLAGS = -Wall -g -DOS_LINUX -DSQLITE_THREADSAFE=2

ifeq ($(PLATFORM), arm)
CFLAGS += -DARM_LINUX
endif

ifeq ($(PLATFORM), ppc)
CFLAGS += -DPPC_LINUX
endif

ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
else
CFLAGS += -UUNIT_TEST
endif

all: $(LIBA)
$(LIBA): buildrepo $(OBJS)
	$(AR) q $@ $(OBJS)
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
	@sh ./postbuild.sh libdb

clean:
	rm -rf $(LIBDIR) $(INCDIR) $(OBJDIR) $(SRCDIR)/*.o
	
install:
	# nothing to do
	
buildrepo:
	@$(call make-repo)

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR)
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(OBJDIR)/$$dir; \
	done
endef
