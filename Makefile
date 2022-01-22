TARGETS = mozo

CXXFILES = trackermanager

BINDIR := ./bin
LIBDIR := ./lib
OBJDIR := ./build
INCDIR := ./include ./benc/include ./openssl/include
SRCDIR := ./src

LINKDIR = ./benc/lib/ ./zlib/lib ./openssl/lib
LINKFILES = libbenc.a

INC = $(foreach d, $(INCDIR), -I$d)
LINK = $(foreach d, $(LINKDIR), -L$d) #$(foreach f, $(LINKFILES), -l:$f)

output := $(shell mkdir -p $(BINDIR) $(LIBDIR) $(OBJDIR))

OFILES = $(patsubst %, $(OBJDIR)/%.o, $(CXXFILES))
EXEFILES = $(patsubst %, $(BINDIR)/%, $(TARGETS))
LIBFILES = 

# dependency files
DFILES = $(patsubst %.o, %.d, $(OFILES))

CXXFLAGS = $(INC) -O3 -MMD -std=c++17 -Wall -Werror
LDFLAGS = $(LINK) -lbenc -lcrypto -lz -pthread -ldl

# make config
.DEFAULT_GOAL = all
.PRECIOUS: $(OFILES)
.PHONY: all clean

AR = ar
ARFLAGS = -rv

# build targets
all: $(EXEFILES) $(LIBFILES)

debug:
	@echo "DEBUG MODE"
debug: CXXFLAGS += -DDEBUG -g
debug: $(EXEFILES) $(LIBFILES)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

$(BINDIR)/%: $(OFILES)
	@echo "[LD] $^ --> $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	@echo "Cleaning up..."
	@rm -rf $(BINDIR) $(LIBDIR) $(OBJDIR) log.txt

-include $(DFILES)