#---------------------------------------------------------------------
# Lconfigs
#---------------------------------------------------------------------
ifndef OUTPUT_ROOT
    OUTPUT_ROOT=.
endif
#CC 				:= g++
#AR				:=ar
#RAN 			:ranlib
ifndef RAN    
	RAN 				:=ranlib
endif
ifndef AR    
	AR 				:=ar
endif

ifndef CC    
	CC 				:= g++
endif
PROJECT_NAME 	:= Dtv.Lib
release_cflags 	:=$(CFLAGS)-fPIC  -O3  -Wall -c
debug_cflags 	:=$(release_cflags) -g -DDEBUG 
RLMODE			:= release
DLMODE			:= debug
DEPLIBS 		:= -lpthread
VFILE := version.txt
VERSION := $(shell cat ${VFILE})
LIB_DIRD 		:= $(OUTPUT_ROOT)/$(MODE)
LIB_DIRD 		:= $(OUTPUT_ROOT)/$(DLMODE)
LIB_DIRR 		:= $(OUTPUT_ROOT)/$(RLMODE)
ifeq ($(STATIC),yes)
	TARGET_LIBD 		:= $(LIB_DIRD)/lib$(PROJECT_NAME).a
	TARGET_LIBR 		:= $(LIB_DIRR)/lib$(PROJECT_NAME).a
	
else     
	LDFLAGS 		:= -shared -fPIC
	TARGET_LIBD 		:= $(LIB_DIRD)/lib$(PROJECT_NAME).so
	TARGET_LIBR 		:= $(LIB_DIRR)/lib$(PROJECT_NAME).so
endif
#---------------------------------------------------------------------
# Cconfigs
#---------------------------------------------------------------------
OBJDIR			:= obj
SRCDIR			:= src
INCSDIR			:= inc
INCDDIR			:= include
MKDIR_P 		:= mkdir -p
CPDIR			:= cp -r
OUT_O_DIR   	:= $(OUTPUT_ROOT)/$(OBJDIR)
OUT_O_DIRD   	:= $(OUTPUT_ROOT)/$(OBJDIR)/$(DLMODE)
OUT_O_DIRR   	:= $(OUTPUT_ROOT)/$(OBJDIR)/$(RLMODE)
OBJSD 			:= $(OUT_O_DIRD)/utils.o 
OBJSR 			:= $(OUT_O_DIRR)/utils.o 
#ar rcs libout.a out.o
#---------------------------------------------------------------------
# Clean config
#---------------------------------------------------------------------
RM 				:= rm -rf

#---------------------------------------------------------------------
# targets
#---------------------------------------------------------------------	

release: USAGE BINFO LDIRR DIR  $(TARGET_LIBR) CP_INC clean

debug:   USAGE BINFO LDIRD DIR  $(TARGET_LIBD) CP_INC clean

all: release

USAGE:
	@echo ""
	@echo "(Usage): make  [<release/debug>] [OUTPUT_ROOT=dir][STATIC=<yes/no>] [standard make option]"
	@echo "  OUTPUT_ROOT: output directory. default = current dir by default \".\\\"" 
	@echo "  STATIC: true to generat a static lib. default = \"false\"."	
	@echo 
	
BINFO:
		@echo "[[[[[[[ Build $(PROJECT_NAME) ver: $(VERSION) ]]]]]]]"
ifeq ($(STATIC),yes)
		@echo "[[[[[[[ Lib type  : STATIC ]]]]]]]"
else     
		@echo "[[[[[[[ Lib type  : SHARED ]]]]]]]"
endif

CP_INC:
	$(MKDIR_P) $(OUTPUT_ROOT)/$(INCDDIR)
	$(CPDIR)   $(INCSDIR)/* $(OUTPUT_ROOT)/$(INCDDIR)
LDIRD:
	$(MKDIR_P) $(LIB_DIRD)
LDIRR:
	$(MKDIR_P) $(LIB_DIRR)
DIR:	
	$(MKDIR_P) $(OUT_O_DIRR)
	$(MKDIR_P) $(OUT_O_DIRD)
	
#---------------------------------------------------------------------
# Shared lib
#---------------------------------------------------------------------
$(TARGET_LIBR):$(OBJSR)
	@echo '>buil lib $(TARGET_LIBR)'
ifeq ($(STATIC),yes)
	$(AR) rcs $@ $(OUT_O_DIRR)/*.o
	$(RAN) $(TARGET_LIBR)
else    
	$(CC) -o $@ $^ $(DEPLIBS) $(LDFLAGS)
endif

$(TARGET_LIBD):$(OBJSD)
	@echo '>buil lib $(TARGET_LIBR)'
ifeq ($(STATIC),yes)
	$(AR) rcs $@ $(OUT_O_DIRD)/*.o
	$(RAN) $(TARGET_LIBR)
else     
	$(CC) -o $@ $^ $(DEPLIBS) $(LDFLAGS)
endif
 

#---------------------------------------------------------------------
# All  targets
#---------------------------------------------------------------------	


$(OUT_O_DIRD)/%.o:$(SRCDIR)/%.cpp 
	@echo '>buil obj $@'	
	$(CC) -o $@ -c $< $(debug_cflags)
	
$(OUT_O_DIRR)/%.o:$(SRCDIR)/%.cpp 
	@echo '>buil obj $@'	
	$(CC) -o $@ -c $< $(release_cflags)	
#---------------------------------------------------------------------
# Clean
#---------------------------------------------------------------------
.PHONY: clean mrproper

clean:
	-$(RM) $(OUT_O_DIR)
	
mrproper: clean
	-$(RM) $(LIB_DIRD)
	-$(RM) $(LIB_DIRR)
	-$(RM) $(OUTPUT_ROOT)/$(INCDDIR)
	