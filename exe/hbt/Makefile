# Target  name
TARGET = hbtester

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib

# Libraries
INC += -I$(PROJBASE)/cm/inc -I$(PROJBASE)/ulp/inc
LIBS =

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.c=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include $(PROJBASE)/cm/global.mk

ifeq ($(PLATFORM), arm)
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -UUNIT_TEST -DARM_LINUX
LIBS += $(PROJBASE)/ulp/lib/libulp_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
else
ifeq ($(PLATFORM), ppc)
CFLAGS += -DOS_LINUX -UUNIT_TEST -DPPC_LINUX
LIBS += $(PROJBASE)/ulp/lib/libulp_ppc.a $(PROJBASE)/cm/lib/libcm_ppc.a 
else
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -UUNIT_TEST
LIBS += $(PROJBASE)/ulp/lib/libulp.a $(PROJBASE)/cm/lib/libcm.a
endif
endif

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CC) $(OBJS) -o $@ -lrt -pthread $(LIBS)
	@mv $(TARGET) $(OBJDIR)

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
