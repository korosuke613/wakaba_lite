##
## atm_server Makefile
##
CC		= g++
LIBS		= -lpq -lpthread -g -std=c++14
SRCS		= atm_server_main.cpp setup_listen.cpp atm_service.cpp balance_func.cpp deposit_func.cpp withdraw_func.cpp transfer_func.cpp common_func.cpp
HDRS            = atm.hpp
OBJS		= $(SRCS:.c=.o)
PROGRAM		= atm_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###
