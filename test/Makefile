# Target  name
TARGET = ulp

# Directories
OBJDIR = obj
SRCDIR = src
DEPDIR = dep
BINDIR = bin

# Libraries
# gtest include
INC += -I$(PROJBASE)/cm/include -I$(PROJBASE)/ulp/inc
# MUST place libnet.a ahead of libcm.a because libnet depends on libcm.a
ifeq ($(PLATFORM), x86)
LIBS = $(PROJBASE)/ulp/lib/libulp.a
else
LIBS = $(PROJBASE)/ulp/lib/libulp_arm.a
endif

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.c=.o))

include $(PROJBASE)/cm/global.mk
CFLAGS += -DOS_LINUX

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CC) $(OBJS) -o $@ -lrt -pthread $(LIBS)
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
