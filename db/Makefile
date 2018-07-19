# Target  name
LIBA = libdb.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

$(shell mkdir -p $(DEPDIR) >/dev/null)

# Libraries
INC += -I$(INCDIR) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc
LIBS =

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.c=.o))
DEPS := $(subst $(SRCDIR)/, $(DEPDIR)/, $(SRCS:.c=.d))

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
	@sh ./postbuild.sh libdb

-include $(DEPS)

clean:
	rm -rf $(LIBDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)
	
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
