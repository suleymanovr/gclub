CXX 	    ?= g++

SRC_DIR     := ./src
INC_DIR     := ./include
BLD_DIR     := ./build

TARGET_EXEC := gclub

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:%=$(BLD_DIR)/%.o)

INC_FLAGS := $(addprefix -I, $(INC_DIR))
SRC_FLAGS := $(INC_FLAGS) -MMD -MP

$(BLD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BLD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(SRC_FLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BLD_DIR)

MKDIR_P ?= mkdir -p
