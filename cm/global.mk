# Complier -fPIC is mandatory when building dynamic lib *.so
CC = $(PLATFORM_CC)
CXX = $(PLATFORM_CXX)
AR = $(PLATFORM_AR)
LD = $(CC)

$(shell mkdir -p $(OBJDIR) >/dev/null)
$(shell mkdir -p $(DEPDIR) >/dev/null)

# For debug load, DON'T define NDEBUG 
#CFLAGS = -Wall -g -O2 -DNDEBUG -fPIC
CFLAGS = -Wall -g -O3 -fPIC -DOS_LINUX
ifeq ($(PLATFORM), arm)
CFLAGS += -DARM_LINUX
endif
ifeq ($(PLATFORM), ppc)
CFLAGS += -DPPC_LINUX
endif

CFLAGS += -DDPE -UTARGET_LOCATION -UTARGET_ANALYSIS -DPHY_DEBUG

LDFLAGS = -lrt

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@set -e; rm -f $@; $(CC) -MM $< $(INC) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INC) $(CFLAGS) -c $< -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INC) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
                
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(INC) $(CFLAGS)  -c $< -o $@
	
%.o: %.cc
	$(CXX) $(INC) $(CFLAGS)  -c $<

