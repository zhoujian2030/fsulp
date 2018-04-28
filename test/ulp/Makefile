# Target  name
TARGET = ulp

# Directories
OBJDIR = obj
SRCDIR = src
DEPDIR = dep
BINDIR = bin

# Libraries
# gtest include
INCGTEST = -I$(PROJBASE)/test/lib/gtest/include
INC += $(INCGTEST) -I$(PROJBASE)/cm/include -I$(PROJBASE)/ulp/inc
# MUST place libnet.a ahead of libcm.a because libnet depends on libcm.a
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/ulp/lib/libulp.a
else
LIBS = $(PROJBASE)/ulp/lib/libulp_arm.a
endif

LIBAGTEST = $(PROJBASE)/test/lib/gtest/lib/libgtest.a $(PROJBASE)/test/lib/gtest/lib/libgtest_main.a
LIBSOGTEST = -L$(PROJBASE)/test/lib/gtest/lib -lgtest -lgtest_main

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

include $(PROJBASE)/cm/global.mk
CFLAGS += -DOS_LINUX -UMAC_PHY_INTF_NEW -DUNIT_TEST

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CXX) $(OBJS) -o $@ -lrt -pthread $(LIBAGTEST) $(LIBS)
	@mv $(TARGET) $(OBJDIR)

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
