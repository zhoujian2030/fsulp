ifeq ($(PLATFORM), arm)
ifeq ($(BUILD_TARGET), nodpe)
SUBDIRS = ulp \
	deamon \
	hbt
else 
SUBDIRS = ulp \
	deamon \
	dpe \
	hbt
endif
else 
ifeq ($(PLATFORM), x86)
SUBDIRS = ulp \
	dpe \
	deamon \
	hbt
else
SUBDIRS = dpe \
	deamon \
	hbt
endif
endif

BUILDDIRS = $(SUBDIRS:%=build-%)
INSTALLDIRS = $(SUBDIRS:%=install-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

# build target
all: $(BUILDDIRS)
$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

# clean target
clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

# install target
install: $(INSTALLDIRS) all
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(INSTALLDIRS)
.PHONY: subdirs $(CLEANDIRS)

.PHONY: all install clean

# specify the dependency here

