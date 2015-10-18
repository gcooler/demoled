BIN    :=  bin
CLIENT :=  $(BIN)/client
SERVER :=  $(BIN)/server

COMMON_SRC  :=  $(wildcard common/*.cpp)
COMMON_OBJ  :=  $(COMMON_SRC:.cpp=.o)

CLIENT_SRC  :=  $(wildcard client/*.cpp)
CLIENT_OBJ  :=  $(CLIENT_SRC:.cpp=.o)

SERVER_SRC  :=  $(wildcard server/*.cpp)
SERVER_OBJ  :=  $(SERVER_SRC:.cpp=.o)

CC          := g++
CPPFLAGS    :=  
CFLAGS      :=  -Wall
LDFLAGS     :=  

.PHONY: all client server clean fclean

all: client server

client: $(CLIENT)
server: $(SERVER)

$(BIN):
	mkdir $@

$(CLIENT): $(COMMON_SRC) $(CLIENT_SRC) | $(BIN)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(SERVER): $(COMMON_SRC) $(SERVER_SRC) | $(BIN)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -r bin
