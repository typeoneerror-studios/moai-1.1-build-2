NEXE_NAME	=	filecallbacks

OBJS 		=						\
				nacl_file.o			\
				geturl_handler.o	\
				file_handler.o		\
				filecallbacks.o			\
				
LDFLAGS		:= $(LDFLAGS) -Wl,--wrap=open,--wrap=read,--wrap=lseek,--wrap=close,--wrap=write

include ../common/Makefile.common
