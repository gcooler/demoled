BIN    :=  bin
CLIENT :=  $(BIN)/client
SERVER :=  $(BIN)/server

CLIENT_SRC :=  $(wildcard client/*.cpp)
CLIENT_HDR :=  $(wildcard client/*.h)

SERVER_SRC :=  $(wildcard server/*.cpp)
SERVER_HDR :=  $(wildcard server/*.h)

CXXFLAGS := -Wall -std=c++0x
LDFLAGS  :=  

.PHONY: all client server clean

all: client server

client: $(CLIENT)
server: $(SERVER)

$(BIN):
	mkdir $@

$(CLIENT): $(CLIENT_SRC) $(CLIENT_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CLIENT_SRC) $(LDLIBS) -o $@

$(SERVER): $(SERVER_SRC) $(SERVER_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SERVER_SRC) $(LDLIBS) -o $@

clean:
	$(RM) -r bin
