
##############################################################################################
#
# our vpaths
#

# all o files in $(OUTPUT_OBJ_DIR)
vpath %.o $(OUTPUT_OBJ_DIR)

# all source files in all of our src,tests,examples,tools,gui dirs

ALL_SRC_DIRS=$(LIB_SRC_DIR) $(LIB_TESTS_DIR) $(LIB_EXAMPLES_DIR) $(LIB_TOOLS_DIR) $(LIB_GUI_DIR) $(LIB_DAEMONS_DIR) $(LIB_TOOLS_DEV_DIR) $(LIB_GUI_DEV_DIR)

vpath %.tex $(LIB_DOCS_DIR)
vpath %.m $(ALL_SRC_DIRS)

vpath %.mm $(ALL_SRC_DIRS)
vpath %.cpp $(ALL_SRC_DIRS)
vpath %.i $(LIB_SWIG_DIR)
vpath %.cc $(ALL_SRC_DIRS)
vpath %.c $(ALL_SRC_DIRS)
vpath %.rc $(ALL_SRC_DIRS)

# all h files in our include dirs
vpath %.h .:$(LIB_INCLUDE_DIR)

# all libraries in our OUTPUT_LIB_DIR
vpath %.a $(OUTPUT_LIB_DIR)
vpath %.so $(OUTPUT_LIB_DIR)
vpath %.dylib $(OUTPUT_LIB_DIR)

# all testing shell scripts in our tests source dir
vpath %.sh $(LIB_TESTS_DIR)

# all object files are precious. Make should not delete them even if they are intermediate
.PRECIOUS : $(OUTPUT_OBJ_DIR)/%.o

# If we are cross compiling, then we must also look in our native dirs for objects and libraries.
ifeq ($(CROSS_COMPILING),1)
vpath %.o $(NATIVE_OUTPUT_OBJ_DIR)
vpath %.a $(NATIVE_OUTPUT_LIB_DIR)
.PRECIOUS : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
endif


