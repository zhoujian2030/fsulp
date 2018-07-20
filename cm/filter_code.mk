# Files and folders
CSRCS   := $(wildcard $(SRCDIR)/*.c)
CCSRCS  := $(wildcard $(SRCDIR)/*.cc)
CPPSRCS := $(wildcard $(SRCDIR)/*.cpp)
COBJS   := $(subst $(SRCDIR)/, $(OBJDIR)/, $(CSRCS:.c=.o))
CCOBJS  := $(subst $(SRCDIR)/, $(OBJDIR)/, $(CCSRCS:.cc=.o))
CPPOBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(CPPSRCS:.cpp=.o))
OBJS := $(COBJS) $(CCOBJS) $(CPPOBJS)
DEPS := $(subst $(OBJDIR)/, $(DEPDIR)/, $(OBJS:.o=.d))
