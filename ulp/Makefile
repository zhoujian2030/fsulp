# Target  name
LIBA = libulp.a

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

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include ../cm/global.mk

CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -UISSUE_DEBUG -DDPE -DHEARTBEAT_DEBUG -DPHY_DEBUG

ifeq ($(PLATFORM), arm)
CFLAGS += -DARM_LINUX
ifeq ($(BUILD_TARGET), nodpe)
CFLAGS += -DENDIAN_CONVERT
endif
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
	@sh ./postbuild.sh libulp

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
