TARGET = 3dstex
CXX = g++
LD  = $(CXX)

CPPFILES := $(wildcard src/*.cpp)
OFILES   := $(CPPFILES:.cpp=.o)
DFILES   := $(OFILES:.o=.d)

CXXFLAGS = -g -O3 -Wall -Wno-misleading-indentation
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
	@rm $(OFILES) $(DFILES) $(TARGET)

-include $(DFILES)
