# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))
WRVX_COMPBASE := $(subst \,/,$(WRVX_COMPBASE))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

JOBS?=1
TARGET_JOBS?=$(JOBS)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = PENTIUMgnu
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := N:/WindRiver/workspace
PRJ_ROOT_DIR := $(WS_ROOT_DIR)/YPopova_CFlory_RTES



#Global Build Macros
PROJECT_TYPE = DKM
DEFINES = 
EXPAND_DBG = 0
ADDED_OBJECTS = ../cinterface.o


#BuildSpec specific Build Macros
VX_CPU_FAMILY = pentium
CPU = PENTIUM
TOOL_FAMILY = gnu
TOOL = gnu
TOOL_PATH = 
CC_ARCH_SPEC = -mtune=pentium -march=pentium -nostdlib -fno-builtin -fno-defer-pop -fno-implicit-fp
VSB_DIR = $(WIND_BASE)/target/lib
VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
LIBPATH = 
LIBS = 

IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 

IDE_LIBRARIES = 

IDE_DEFINES = -DCPU=_VX_$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   -DIP_PORT_VXWORKS=69 



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = -g
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler =  -O2
DEBUGFLAGS_C++-Compiler =  -O2
DEBUGFLAGS_Linker =  -O2
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler =  -O2
endif


#Project Targets
PROJECT_TARGETS = YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES.out \
	 YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage.o


#Rules

# YPopova_CFlory_RTES
ifeq ($(DEBUG_MODE),1)
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler =  -O2
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler =  -O2
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Linker =  -O2
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEBUGFLAGS_Assembler =  -O2
endif
YPopova_CFlory_RTES/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 
YPopova_CFlory_RTES/$(MODE_DIR)/% : IDE_LIBRARIES = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : IDE_DEFINES = -DCPU=_VX_$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   -DIP_PORT_VXWORKS=69 
YPopova_CFlory_RTES/$(MODE_DIR)/% : PROJECT_TYPE = DKM
YPopova_CFlory_RTES/$(MODE_DIR)/% : DEFINES = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : EXPAND_DBG = 0
YPopova_CFlory_RTES/$(MODE_DIR)/% : ADDED_OBJECTS = ../cinterface.o
YPopova_CFlory_RTES/$(MODE_DIR)/% : VX_CPU_FAMILY = pentium
YPopova_CFlory_RTES/$(MODE_DIR)/% : CPU = PENTIUM
YPopova_CFlory_RTES/$(MODE_DIR)/% : TOOL_FAMILY = gnu
YPopova_CFlory_RTES/$(MODE_DIR)/% : TOOL = gnu
YPopova_CFlory_RTES/$(MODE_DIR)/% : TOOL_PATH = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : CC_ARCH_SPEC = -mtune=pentium -march=pentium -nostdlib -fno-builtin -fno-defer-pop -fno-implicit-fp
YPopova_CFlory_RTES/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
YPopova_CFlory_RTES/$(MODE_DIR)/% : VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
YPopova_CFlory_RTES/$(MODE_DIR)/% : LIBPATH = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : LIBS = 
YPopova_CFlory_RTES/$(MODE_DIR)/% : OBJ_DIR := YPopova_CFlory_RTES/$(MODE_DIR)

OBJECTS_YPopova_CFlory_RTES = YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage.o

ifeq ($(TARGET_JOBS),1)
YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES.out : $(OBJECTS_YPopova_CFlory_RTES)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@";rm -f "$@";nmpentium $(OBJECTS_YPopova_CFlory_RTES) | tclsh $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c pentium -tags $(VSB_DIR)/tags/pentium/PENTIUM/common/dkm.tags > $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)ccpentium $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -fdollars-in-identifiers -Wall   $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES)  $(IDE_DEFINES) $(DEFINES) -o $(OBJ_DIR)/ctdt.o -c $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)ccpentium -r -nostdlib -Wl,-X -T $(WIND_BASE)/target/h/tool/gnu/ldscripts/link.OUT -o "$@" $(OBJ_DIR)/ctdt.o $(OBJECTS_YPopova_CFlory_RTES) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

else
YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES.out : YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES.out_jobs

endif
YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES_compile_file : $(FILE) ;

_clean :: YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES_clean

YPopova_CFlory_RTES/$(MODE_DIR)/YPopova_CFlory_RTES_clean : 
	$(TRACE_FLAG)if [ -d "YPopova_CFlory_RTES" ]; then cd "YPopova_CFlory_RTES"; rm -rf $(MODE_DIR); fi


# YPopova_CFlory_RTES_partialImage
ifeq ($(DEBUG_MODE),1)
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler =  -O2
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler =  -O2
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker =  -O2
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler =  -O2
endif
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : IDE_LIBRARIES = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : IDE_DEFINES = -DCPU=_VX_$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   -DIP_PORT_VXWORKS=69 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : PROJECT_TYPE = DKM
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : DEFINES = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : EXPAND_DBG = 0
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : ADDED_OBJECTS = ../cinterface.o
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : VX_CPU_FAMILY = pentium
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : CPU = PENTIUM
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : TOOL_FAMILY = gnu
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : TOOL = gnu
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : TOOL_PATH = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : CC_ARCH_SPEC = -mtune=pentium -march=pentium -nostdlib -fno-builtin -fno-defer-pop -fno-implicit-fp
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : LIBPATH = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : LIBS = 
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/% : OBJ_DIR := YPopova_CFlory_RTES_partialImage/$(MODE_DIR)

YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/Objects/YPopova_CFlory_RTES/converyorBelt.o : $(PRJ_ROOT_DIR)/converyorBelt.c $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccpentium $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -ansi -fno-zero-initialized-in-bss  -Wall    -MD -MP $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


OBJECTS_YPopova_CFlory_RTES_partialImage = YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/Objects/YPopova_CFlory_RTES/converyorBelt.o

ifeq ($(TARGET_JOBS),1)
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage.o : $(OBJECTS_YPopova_CFlory_RTES_partialImage)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccpentium -r -nostdlib -Wl,-X  -o "$@" $(OBJECTS_YPopova_CFlory_RTES_partialImage) $(ADDED_OBJECTS) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

else
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage.o : YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage.o_jobs

endif
YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage_compile_file : $(FILE) ;

_clean :: YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage_clean

YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/YPopova_CFlory_RTES_partialImage_clean : 
	$(TRACE_FLAG)if [ -d "YPopova_CFlory_RTES_partialImage" ]; then cd "YPopova_CFlory_RTES_partialImage"; rm -rf $(MODE_DIR); fi

force : 

TARGET_JOBS_RULE?=echo "Update the makefile template via File > Import > Build Settings : Update makefile template";exit 1
%_jobs : 
	$(TRACE_FLAG)$(TARGET_JOBS_RULE)

DEP_FILES := YPopova_CFlory_RTES_partialImage/$(MODE_DIR)/Objects/YPopova_CFlory_RTES/converyorBelt.d
-include $(DEP_FILES)


WIND_SCOPETOOLS_BASE := $(subst \,/,$(WIND_SCOPETOOLS_BASE))

clean_scopetools :
	$(TRACE_FLAG)rm -rf $(PRJ_ROOT_DIR)/.coveragescope/db

CLEAN_STEP := clean_scopetools


-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

TARGET_JOBS_RULE=$(MAKE) -f $(MAKEFILE) --jobs $(TARGET_JOBS) $(MFLAGS) $* TARGET_JOBS=1
ifeq ($(JOBS),1)
main_all : external_build  $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"
else
main_all : external_build 
	@$(MAKE) -f $(MAKEFILE) --jobs $(JOBS) $(MFLAGS) $(PROJECT_TARGETS) TARGET_JOBS=1 &&\
	echo "make: built targets of `pwd`"
endif 

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
