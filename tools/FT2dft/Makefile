# FT2dft Makefile

EXEC		:= FT2dft

SOURCES		:= source	../../libdft/source
INCLUDES	:= include	../../libdft/include
LIBS		:= 

# ---- don't edit past this line ----

INCLUDES	+= /usr/include/freetype2
LIBS		+= -lfreetype -lm

CFILES		:= $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
INCLUDE		:= $(foreach dir,$(INCLUDES),-I$(dir))

.PHONY:all clean

all:$(EXEC)

$(EXEC):$(CFILES)
	gcc -g -o $@ $(INCLUDE) $(LIBS) $^

clean:
	rm -fr $(EXEC)

