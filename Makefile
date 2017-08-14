.SUFFIXES:
.DEFAULT: all
.NOTPARALLEL:
.SILENT:

.PHONY: all clean test

BLDDIR := build
DEPDIR := $(BLDDIR)/.d

SRCS := test_math.cpp tests/unit_test_fvec4.cpp tests/unit_test_lvec4.cpp

CFLAGS_sse2 := -msse2
CFLAGS_sse3 := -msse3
CFLAGS_sse4 := -msse4.1
CFLAGS_native := -march=native -mtune=native
CONFIGS := sse2 sse3 sse4 native

TARGS := $(addprefix $(BLDDIR)/,$(foreach t,$(basename $(notdir $(SRCS))),$(foreach c,$(CONFIGS),$(t)_$(c))))
$(info $(basename $(notdir $(SRCS))))
$(info $(TARGS))
define TARGRULE

$(BLDDIR)/$(basename $(notdir $(1)))_$(2): $(1) | $(DEPDIR) $(BLDDIR)
	echo "[CC $(2)] $$@"
	g++ $(CFLAGS_$(2)) -MT $$@ -MMD -MP -MF $$(patsubst $(BLDDIR)%,$(DEPDIR)%,$$@.Td) --std=c++11 -I$(CURDIR) -o $$@ $(1)
	mv -f $$(patsubst $(BLDDIR)%,$(DEPDIR)%,$$@.Td) $$(patsubst $(BLDDIR)%,$(DEPDIR)%,$$@.d) && touch $$@

endef

define RUNTESTTARGRULE

.PHONY: $(patsubst $(BLDDIR)/unit_test_%,run_test_%,$(1))
$(patsubst $(BLDDIR)/unit_test_%,run_test_%,$(1)): $(1)
	$(1)

test: $(patsubst $(BLDDIR)/unit_test_%,run_test_%,$(1))

TEST_NAMES += $(patsubst $(BLDDIR)/unit_test_%,run_test_%,$(1))
endef

all: $(TARGS) | $(BLDDIR)

$(DEPDIR):
	mkdir -p $(DEPDIR)

$(BLDDIR):
	mkdir -p $(BLDDIR)

### DEBUG: $(foreach c,$(CONFIGS),$(foreach T,$(SRCS),$(info $(call TARGRULE,$(T),$(c)))))
$(foreach c,$(CONFIGS),$(foreach T,$(SRCS),$(eval $(call TARGRULE,$(T),$(c)))))

TEST_TARGS:=$(filter $(BLDDIR)/unit_test%,$(TARGS))
TEST_NAMES:=#
### DEBUG: $(info $(TEST_TARGS))

$(foreach T,$(TEST_TARGS),$(eval $(call RUNTESTTARGRULE,$(T))))

$(info $(TEST_NAMES))

clean:
	rm -rf $(DEPDIR) $(BLDDIR)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(notdir $(TARGS))))
