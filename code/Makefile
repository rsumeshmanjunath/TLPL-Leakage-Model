CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra -Wpedantic

SRCDIR = .
BUILDDIR = build
TARGET = main

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	@rm -rf $(BUILDDIR) $(TARGET)

