# Target  name
LIBA = libdpe.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib

# Libraries
INC += -I$(PROJBASE)/cm/inc -I$(PROJBASE)/db/inc -I$(PROJBASE)/ulp/inc
LIBS = 

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include $(PROJBASE)/cm/global.mk
CFLAGS += -Wno-write-strings

ifeq ($(PLATFORM), arm)
CFLAGS += -DOS_LINUX -UUNIT_TEST -DARM_LINUX
LIBS += $(PROJBASE)/db/lib/libdb_arm.a $(PROJBASE)/cm/lib/libcm_arm.a
else
CFLAGS += -DOS_LINUX -UUNIT_TEST
LIBS += $(PROJBASE)/db/lib/libdb.a $(PROJBASE)/cm/lib/libcm.a 
endif

all: $(LIBA)
ifeq ($(PLATFORM), arm)
$(LIBA): buildrepo $(OBJS)
	$(AR) q $@ $(OBJS)
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
	@sh ./postbuild.sh libdpe
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
