# Target  name
LIBA = libdpe.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
DEPDIR = .d

$(shell mkdir -p $(DEPDIR) >/dev/null)

# Libraries
INC += -I$(INCDIR) -I$(PROJBASE)/cm/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/ulp/inc
LIBS = 

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS := $(subst $(SRCDIR)/, $(DEPDIR)/, $(SRCS:.cpp=.d))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include $(PROJBASE)/cm/global.mk
CFLAGS += -Wno-write-strings -DOS_LINUX -UCOLLECT_IMSI -UENDIAN_CONVERT

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
	@sh ./postbuild.sh libdpe

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
