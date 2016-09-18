TARGET = 3dstex
LD = $(CXX)

CPPFILES := $(wildcard src/*.cpp)
OFILES   := $(CPPFILES:.cpp=.o)
DEPENDS  := $(OFILES:.o=.d)

CXXFLAGS := $(CXXFLAGS) -g -O3 -std=c++11 -Wall -pedantic -Wno-misleading-indentation
LDFLAGS  := $(LDFLAGS) $(CXXFLAGS) -Wall
LIBS     := -lm

.PHONY: all clean run test $(TARGET)

all: $(TARGET)

%.o: %.cpp
	@echo Compiling $<
	$(CXX) -MMD -MP -MF $*.d $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	@echo Linking $(TARGET) ...
	$(LD) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

test:
	@echo Running tests...
	
clean:
	@rm $(OFILES) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
