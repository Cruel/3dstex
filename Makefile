TARGET = 3dstex
LD = $(CXX)

BUILD_DIR:= build
CPPFILES := $(wildcard src/*.cpp)
OFILES   := $(patsubst src/%,$(BUILD_DIR)/%,$(CPPFILES:.cpp=.o))
DEPENDS  := $(wildcard $(BUILD_DIR)/*.d)

CXXFLAGS := $(CXXFLAGS) -g -O3 -std=c++11 -Wall -pedantic -Wno-misleading-indentation
LDFLAGS  := $(LDFLAGS) $(CXXFLAGS) -Wall
LIBS     := -lm

.PHONY: all clean test $(TARGET)

all: $(TARGET)

$(OFILES): | dir

dir:
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) -MMD -MP -MF $(BUILD_DIR)/$*.d $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	$(LD) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

test:
	@$(MAKE) -C test
	
clean:
	@rm -rf $(BUILD_DIR) $(TARGET)
	@$(MAKE) -C test clean

-include $(DEPENDS)
