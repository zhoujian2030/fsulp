# Target  name
TARGET = dpe

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
DEPDIR = dep
BINDIR = bin
DEPDIR = .d

$(shell mkdir -p $(DEPDIR) >/dev/null)

# Libraries
# gtest include
INCGTEST = -I$(PROJBASE)/test/lib/gtest/include
INC += -I$(INCDIR) $(INCGTEST) -I$(PROJBASE)/cm/extinc -I$(PROJBASE)/cm/inc -I$(PROJBASE)/dpe/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/ulp/inc
# MUST place libnet.a ahead of libcm.a because libnet depends on libcm.a
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/dpe/lib/libdpe.a $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a
else
LIBS = $(PROJBASE)/dpe/lib/libdpe_arm.a $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
endif

LIBAGTEST = $(PROJBASE)/test/lib/gtest/lib/libgtest.a $(PROJBASE)/test/lib/gtest/lib/libgtest_main.a
LIBSOGTEST = -L$(PROJBASE)/test/lib/gtest/lib -lgtest -lgtest_main

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS := $(subst $(SRCDIR)/, $(DEPDIR)/, $(SRCS:.cpp=.d))

include $(PROJBASE)/cm/global.mk
CFLAGS += -Wno-write-strings -DOS_LINUX -UMAC_PHY_INTF_NEW -DUNIT_TEST -UISSUE_DEBUG

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CXX) $(OBJS) -o $@ -lrt -pthread $(LIBAGTEST) $(LIBS) -ldl
	@mv $(TARGET) $(OBJDIR)

-include $(DEPS)

clean:
	rm -rf $(BINDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)

install:
	mkdir -p $(BINDIR)
	cp $(OBJDIR)/$(TARGET) $(BINDIR)
	
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
