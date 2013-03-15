NEXE_NAME	=	effects

OBJS 		=						\
				nacl_file.o			\
				geturl_handler.o	\
				file_handler.o		\
				effects.o			\
				
LDFLAGS		:= $(LDFLAGS) -Wl,--wrap=open,--wrap=read,--wrap=lseek,--wrap=close,--wrap=write

include ../common/Makefile.common
