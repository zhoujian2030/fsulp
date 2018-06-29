# Target  name
TARGET = dpe

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib

# Libraries
INC += -I$(PROJBASE)/cm/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/dpe/inc -I$(PROJBASE)/ulp/inc
LIBS = 

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include $(PROJBASE)/cm/global.mk

ifeq ($(PLATFORM), arm)
CFLAGS += -DOS_LINUX -UUNIT_TEST -DARM_LINUX
LIBS += $(PROJBASE)/dpe/lib/libdpe_arm.a $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a 
else
CFLAGS += -DOS_LINUX -UUNIT_TEST
LIBS += $(PROJBASE)/dpe/lib/libdpe.a $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a
endif

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CXX) $(OBJS) -o $@ -lrt -pthread $(LIBS) -ldl
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
