TARGET = 3dstex-test
LD = $(CXX)

BUILD_DIR:= build
CPPFILES := $(wildcard ../src/*.cpp)
OFILES   := $(patsubst ../src/%,$(BUILD_DIR)/%,$(CPPFILES:.cpp=.o))
DEPENDS  := $(wildcard $(BUILD_DIR)/*.d)

CXXFLAGS := -g -std=c++11 --coverage
LDFLAGS  := $(CXXFLAGS) -Wall
LIBS     := -lm

.PHONY: all clean run $(TARGET)

all: run

$(OFILES): | dir

dir:
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: ../src/%.cpp
	$(CXX) -MMD -MP -MF $(BUILD_DIR)/$*.d $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	$(LD) $(OFILES) $(LDFLAGS) $(LIBS) -o $@
	
run: $(TARGET)
	@bash -e test.sh
	
clean:
	@rm -rf $(BUILD_DIR) $(TARGET) *.bin *.png

-include $(DEPENDS)
