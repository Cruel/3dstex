TARGET = 3dstex
LD = $(CXX)

CPPFILES := $(wildcard src/*.cpp)
OFILES   := $(CPPFILES:.cpp=.o)
DEPENDS  := $(OFILES:.o=.d)

CXXFLAGS = -g -O3 -std=c++11 -Wall -pedantic -Wno-misleading-indentation
LDFLAGS  = -Wall -Wl,-Map,$(TARGET).map
LIBS     = -lm

.PHONY: all clean run $(TARGET)

all: $(TARGET)

%.o: %.cpp
	@echo Compiling $<
	@$(CXX) -MMD -MP -MF $*.d $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	@echo Linking $(TARGET) ...
	@$(LD) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

clean:
	@rm $(OFILES) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
