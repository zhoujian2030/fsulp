all: $(TARGET)

# Targets
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo $(TARGET): $(OBJS) $(LIBS) > $(DEPDIR)/$(TARGET).d
	

clean:
	rm -rf $(TARGET) $(LIBDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)
	
install:
	# nothing to do

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
-include $(DEPDIR)/$(TARGET).d
endif
