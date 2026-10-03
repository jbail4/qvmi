SRC_DIR = src
BUILD_DIR = build
CFLAGS = -Wall -g -fPIC
LDFLAGS = -shared
INSTALL_DIR = /usr/local/lib
LIB_NAME = libqvmi.so

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/$(LIB_NAME)

all: $(TARGET)

$(TARGET) : $(OBJ)
	gcc $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

install: $(TARGET)
	sudo cp $< $(INSTALL_DIR)
	sudo ldconfig

test:
	rm -rf test
	gcc test.c -o test -I. -Lbuild -lqvmi

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean test