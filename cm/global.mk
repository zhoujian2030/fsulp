# Complier -fPIC is mandatory when building dynamic lib *.so
CC = $(PLATFORM_CC)
CXX = $(PLATFORM_CXX)
AR = $(PLATFORM_AR)

# For debug load, DON'T define NDEBUG 
#CFLAGS = -Wall -g -O2 -DNDEBUG -fPIC
CFLAGS = -Wall -g -O2 -fPIC

LFLAGS = -lrt

.%.d: %.cpp
	$(CXX) $(INC) -MM $< > $@
	@$(CXX) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INC) $(CFLAGS) -c $< -o $@

.%.d: %.c
	$(CC) $(INC) -MM $< > $@
	@$(CC) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
                
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(INC) $(CFLAGS)  -c $< -o $@
	
%.o: %.cc
	$(CXX) $(INC) $(CFLAGS)  -c $<
