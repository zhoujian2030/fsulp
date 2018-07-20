$(shell mkdir -p $(LIBDIR) >/dev/null)

POST_BUILD=@sh ./postbuild.sh $(LIBNAME)
LIBA=$(LIBDIR)/$(LIBNAME).a

all: $(LIBA)

$(LIBA): $(OBJS)
	$(AR) q $@ $^
	$(POST_BUILD)

clean:
	rm -rf $(LIBA) $(LIBDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)
	
install:
	# nothing to do

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
