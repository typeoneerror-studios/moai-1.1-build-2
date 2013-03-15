NEXE_NAME	=	3d_reverb

OBJS 		=						\
				nacl_file.o			\
				geturl_handler.o	\
				file_handler.o		\
				3d_reverb.o		\
				
LDFLAGS		:= $(LDFLAGS) -Wl,--wrap=open,--wrap=read,--wrap=lseek,--wrap=close,--wrap=write

include ../common/Makefile.common
