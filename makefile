# Compiler options
CC = g++
OPTCFLAGS = -O2
CFLAGS = -Wall -std=c++11 $(OPTCFLAGS)
LDFLAGS = -pthread -static-libstdc++ -lz

# Directory organisation
BASEDIR = .
BIN = $(BASEDIR)/bin
BUILD = $(BASEDIR)/build
LIBBUILD = $(BASEDIR)/build
INCLUDE = $(BASEDIR)/include
SRC = $(BASEDIR)/src
CPP = $(wildcard $(SRC)/*.cpp) $(wildcard $(SRC)/*/*.cpp)
LIBCPP = $(wildcard $(INCLUDE)/*/*.cpp)
BWAC = $(wildcard $(INCLUDE)/bwa/*.c)

# Target
TARGET = radsex

# Variables
OBJS = $(addprefix $(BUILD)/, $(notdir $(CPP:.cpp=.o)))
LIBOBJS = $(LIBCPP:.cpp=.o)
BWAOBJS = $(BWAC:.c=.o)

# Rules

all: init BWA $(TARGET)

print-%: ; @echo $* = $($*)

$(TARGET): $(OBJS) $(LIBOBJS) $(BWAOBJS) $(INCLUDE)/bwa/libbwa.a
	$(CC) $(CFLAGS) -I $(INCLUDE) -L $(INCLUDE)/bwa -lbwa -o $(BIN)/$(TARGET) $^ $(LDFLAGS)

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -I $(INCLUDE) -c -o $@ $^

$(LIBBUILD)/%.o: $(INCLUDE)/*/%.cpp
	$(CC) $(CFLAGS) -I $(INCLUDE) -c -o $@ $^

clean:
	@rm -rf $(BUILD)/*.o
	@rm -rf $(BIN)/$(TARGET)
	@rm -rf $(INCLUDE)/*/*.o
	@cd $(INCLUDE)/bwa && $(MAKE) clean

init:
	@mkdir -p $(BUILD) $(BUILD)
	@mkdir -p $(BIN) $(BIN)

rebuild: clean BWA $(TARGET)

BWA:
	@cd $(INCLUDE)/bwa && $(MAKE)
