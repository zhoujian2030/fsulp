$(shell mkdir -p $(LIBDIR) >/dev/null)

POST_BUILD=@sh ./postbuild.sh $(LIBNAME)
LIBA=$(LIBDIR)/$(LIBNAME).a

all: $(LIBA)

$(LIBA): $(OBJS)
	$(AR) rcs $@ $^
	@echo $(LIBNAME).a: $(OBJS) > $(DEPDIR)/$(LIBNAME).d
	$(POST_BUILD)

clean:
	rm -rf $(LIBA) $(LIBDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)
	
install:
	# nothing to do

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
-include $(DEPDIR)/$(LIBNAME).d
endif
