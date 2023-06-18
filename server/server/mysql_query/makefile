COMPILER = clang++
INCLUDE = /usr/local/include
LIBS = /usr/local/lib/mysql
LINKER_OPT = -lmysqlclient
SRC_FILES = src/main.cpp src/mysql_handler.cpp

all:
	$(COMPILER) -o mysql_query -I$(INCLUDE) -L$(LIBS) $(LINKER_OPT) $(SRC_FILES)
