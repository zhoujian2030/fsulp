# Target  name
LIBA = libcm.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
EXTERNAL_INC = include

# Libraries
INC += 
LIBS =

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.c=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include global.mk

ifeq ($(PLATFORM), arm)
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -DARM_LINUX -DRT_LOGGER
else
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -DRT_LOGGER
endif

ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
endif

all: $(LIBA)
$(LIBA): buildrepo $(OBJS)
	$(AR) q $@ $(OBJS)
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
	@sh ./postbuild.sh libcm

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
