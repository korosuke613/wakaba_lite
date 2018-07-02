##
## atm_server Makefile
##
PROGRAM		= wakaba_lite
CC		= g++-7
SRCDIR		:= src
INCDIR		:= include
OUTDIR		:= build
LIBS		= -lpq -lpthread -g -std=c++14 -Iinclude
SRCS		= $(wildcard $(SRCDIR)/*.cpp)
OBJS		= $(addprefix $(OUTDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
TARGET		= $(OUTDIR)/$(PROGRAM)

all:		$(TARGET)

$(TARGET):	$(OBJS) 
		$(CC) $(LIBS) -o $@ $^

$(OUTDIR)/%.o:%.cpp
		@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
		$(CC) $(LIBS) -o $@ -c $<

clean:
		rm -rf $(OUTDIR)

###
