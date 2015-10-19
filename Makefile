BIN    :=  bin
CLIENT :=  $(BIN)/client
SERVER :=  $(BIN)/server

COMMON_SRC :=  $(wildcard common/*.cpp)
COMMON_SRC :=  $(wildcard common/*.h)

CLIENT_SRC :=  $(wildcard client/*.cpp)
CLIENT_HDR :=  $(wildcard client/*.h)

SERVER_SRC :=  $(wildcard server/*.cpp)
SERVER_HDR :=  $(wildcard server/*.h)

CXXFLAGS := -Wall  
LDFLAGS  :=  

.PHONY: all client server clean

all: client server

client: $(CLIENT)
server: $(SERVER)

$(BIN):
	mkdir $@

$(CLIENT): $(COMMON_SRC) $(CLIENT_SRC) $(COMMON_HDR) $(CLIENT_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(COMMON_SRC) $(CLIENT_SRC) $(LDLIBS) -o $@

$(SERVER): $(COMMON_SRC) $(SERVER_SRC) $(COMMON_HDR) $(SERVER_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(COMMON_SRC) $(SERVER_SRC) $(LDLIBS) -o $@

clean:
	$(RM) -r bin
