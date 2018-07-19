# Target  name
TARGET = deamon

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

$(shell mkdir -p $(DEPDIR) >/dev/null)

# Libraries
INC += -I$(INCDIR)
LIBS =

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS := $(subst $(SRCDIR)/, $(DEPDIR)/, $(SRCS:.cpp=.d))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include $(PROJBASE)/cm/global.mk
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -UUNIT_TEST

ifeq ($(PLATFORM), arm)
CFLAGS += -DARM_LINUX
endif

ifeq ($(PLATFORM), ppc)
CFLAGS += -DPPC_LINUX
endif

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CXX) $(OBJS) -o $@ -lrt -pthread $(LIBS)
	@mv $(TARGET) $(OBJDIR)

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
