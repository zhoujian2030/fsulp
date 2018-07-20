all: $(TARGET)

# Targets
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)
	@mv $(TARGET) $(OBJDIR)

clean:
	rm -rf $(TARGET) $(LIBDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)
	
install:
	# nothing to do

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
