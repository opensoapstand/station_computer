TARGET_EXEC ?= fsm

BUILD_DIR ?= ./build
SRC_DIRS ?= ./ ./components ./tinyxml ./objects ./states

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p





# CC     	:= gcc
# CXX    	:= g++
# LIBS		:=
# INCS 		:=
# CFLAGS   := -Wall 

# OUTPUT 	:= fsm

# all: ${OUTPUT}

# #****************************************************************************
# # Source files
# #****************************************************************************

# SRCS := 	./components/gpio.cpp \
# 			./components/i2cgpio.cpp\
# 			./components/mcp23017.cpp\
# 			./components/mcpgpio.cpp\
# 			./components/oddyseyx86gpio.cpp\
# 			./components/smbuc.c\
# 			./states/statevirtual.cpp\
# 			./states/stateinit.cpp\
# 			./objects/debugoutput.cpp\
# 			./objects/messagemediator.cpp\
# 			fsm.cpp

# # Add on the sources for libraries
# SRCS := ${SRCS}

# OBJS := $(addsuffix .o,$(basename ${SRCS}))

# #****************************************************************************
# # common rules
# #****************************************************************************

# # Rules for compiling source files to object files
# %.o : %.cpp
# 	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

# %.o : %.c
# 	${CC} -c ${CFLAGS} ${INCS} $< -o $@


# clean:
# 	-rm -f core ${OBJS} ${OUTPUT}