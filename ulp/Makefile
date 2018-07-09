# Target  name
LIBA = libulp.a

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

CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -UISSUE_DEBUG -DDPE -DHEARTBEAT_DEBUG -DPHY_DEBUG

ifeq ($(PLATFORM), arm)
CFLAGS += -DARM_LINUX
LIBS += $(PROJBASE)/cm/extlib/libnetcp.a
endif

ifeq ($(GTEST_SUPPORT), TRUE)
CFLAGS += -DUNIT_TEST
endif

all: $(LIBA)
ifeq ($(PLATFORM), arm)
$(LIBA): buildrepo $(OBJS)
	$(AR) x $(LIBS) 
	@mv *.o $(OBJDIR)
	@mv *.ov7A $(OBJDIR)
	$(AR) q $@ $(OBJDIR)/*.*
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
	@sh ./postbuild.sh libulp
else
$(LIBA): buildrepo $(OBJS)
	$(AR) q $@ $(OBJS)
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
endif

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
