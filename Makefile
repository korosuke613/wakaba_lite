##
## atm_server Makefile
##
CC		= g++
LIBS		= -lpq
SRCS		= atm_server_main.c setup_listen.c atm_service.c balance_func.c deposit_func.c withdraw_func.c transfer_func.c common_func.c
HDRS            = atm.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= atm_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###
