BUILD_DIR = build
CC = g++
CFLAGS = -c -Wall
LDFLAGS =
SOURCES = server.cpp client.cpp util.cpp
OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLES = $(BUILD_DIR)/server $(BUILD_DIR)/client

all: $(EXECUTABLES)

# Build the server executable
$(BUILD_DIR)/server: $(BUILD_DIR)/server.o $(BUILD_DIR)/util.o
	$(CC) $(LDFLAGS) $^ -o $@

# Build the client executable
$(BUILD_DIR)/client: $(BUILD_DIR)/client.o $(BUILD_DIR)/util.o
	$(CC) $(LDFLAGS) $^ -o $@

# Compile the C++ source files
$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

# Remove all object files and executables
clean:
	rm -rf $(BUILD_DIR)

# Create the build directory
$(shell mkdir -p $(BUILD_DIR))

# Run the server executable
run-server: $(BUILD_DIR)/server
	$(BUILD_DIR)/server

# Run the client executable
run-client: $(BUILD_DIR)/client
	$(BUILD_DIR)/client
