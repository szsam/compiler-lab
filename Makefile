LEX = flex
YACC = bison
CXX = g++

TARGET_EXEC ?= parser

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

LFILE := $(SRC_DIRS)/lexical.l
YFILE := $(SRC_DIRS)/syntax.y
LCFILE := $(LFILE:%.l=%.cpp)
YCFILE := $(YFILE:%.y=%.tab.cpp)
YHFILE := $(YFILE:%.y=%.tab.h)

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
SRCS := $(filter-out $(LCFILE) $(YCFILE), $(SRCS))
SRCS += $(LCFILE) $(YCFILE)

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -std=c++14
LDFLAGS ?= -lfl -ly

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp $(YHFILE)
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# lex
$(LCFILE): $(LFILE)
	@$(MKDIR_P) $(dir $@)
	$(LEX) -o $(LCFILE) $(LFILE)

# yacc
$(YHFILE) $(YCFILE): $(YFILE)
	@$(MKDIR_P) $(dir $@)
	$(YACC) -v $(YFILE) --defines=$(YHFILE) -o $(YCFILE)

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) $(LCFILE) $(YCFILE) $(YHFILE)

-include $(DEPS)

MKDIR_P ?= mkdir -p

# parser : main.c syntax.tab lex.yy.c parse_tree.c parse_tree.h
# 	g++ -std=c++14 -g main.c lex.yy.c syntax.tab.c parse_tree.c -lfl -ly -o parser
# 
# lex.yy.c : lexical.l syntax.tab
# 	flex lexical.l
# syntax.tab : syntax.y
# 	bison -d -v -t syntax.y
# clean:
# 	rm -rf parser lex.yy.c syntax.tab* syntax.output
