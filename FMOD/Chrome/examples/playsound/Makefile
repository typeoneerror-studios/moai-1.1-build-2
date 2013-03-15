NEXE_NAME	=	playsound

OBJS 		=						\
				nacl_file.o			\
				geturl_handler.o	\
				file_handler.o		\
				playsound.o			\
				
LDFLAGS		:= $(LDFLAGS) -Wl,--wrap=open,--wrap=read,--wrap=lseek,--wrap=close,--wrap=write

include ../common/Makefile.common
