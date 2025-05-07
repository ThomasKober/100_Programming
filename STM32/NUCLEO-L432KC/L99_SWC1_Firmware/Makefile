#-----------------------------------------------------------------------------------------------------------------------
#
# CONFIG
#
#-----------------------------------------------------------------------------------------------------------------------
BUILDDIR_HOST  = build
TARGETDIR_HOST = dist
Target         = TestOutput #for map file.. needs to be improved

HEADER =  -I ./test/src \
          -I ./test/inc \
          -I ./utilities/unity/src/ \
          -I ./utilities/fff/ \
          -I ./source/gLib/gCore \
          -I ./source/gLib/gApp \
          -I ./source/Core/Inc \
          -I ./source/Drivers/STM32H5xx_HAL_Driver/Inc \
          -I ./source/Drivers/STM32H5xx_HAL_Driver/Inc/Legacy \
          -I ./source/Drivers/CMSIS/Include \
          -I ./source/Drivers/CMSIS/Device/ST/STM32H5xx/Include

VPATH =   ./source/gLib \
          ./source/gLib/gApp \
          ./source/gLib/gCore \
          ./source/Core/Src \
          ./utilities/unity/src \
          ./source/Drivers/STM32H5xx_HAL_Driver/Src \
          ./test/src

TEST_SOURCES:=$(wildcard ./test/src/*.c)

#-----------------------------------------------------------------------------------------------------------------------
# HOST  COMPILER/LINKER
#-----------------------------------------------------------------------------------------------------------------------
GCC_CC = gcc
#-----------------------------------------------------------------------------------------------------------------------
# COMPILER FLAGS
#-----------------------------------------------------------------------------------------------------------------------
GCC_CCFLAGS = -Wall -fprofile-arcs -ftest-coverage -DUNIT_TEST -DACTIVATE_TEST_INTERFACE -DUSE_HAL_DRIVER -DSTM32H562xx $(HEADER)
#-----------------------------------------------------------------------------------------------------------------------
# LINKER FLAGS
#-----------------------------------------------------------------------------------------------------------------------
GCC_LDFLAGS = $(GCC_CCFLAGS)

#-----------------------------------------------------------------------------------------------------------------------
#
# TEST TARGETS
#
#-----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------------
# TEST GAPP
# ----------------------------------------------------------------------------------------------------------------------------
TEST_GAPP_TARGET_HOST = $(TARGETDIR_HOST)/test_gApp.exe
TEST_GAPP__OBJS_HOST = test_gApp.o gapp.o unity.o
TEST_GAPP_OBJS_HOST = $(patsubst %,$(BUILDDIR_HOST)/%,$(TEST_GAPP__OBJS_HOST))


#----------------------------------------------------------------------------------------------------------------------------
#
# RULES
#
#----------------------------------------------------------------------------------------------------------------------------

host: $(TEST_GAPP_TARGET_HOST)

#----------------------------------------------------------------------------------------------------------------------------
# BUILD HOST
#----------------------------------------------------------------------------------------------------------------------------
#Link HOST
$(TEST_GAPP_TARGET_HOST): $(TEST_GAPP_OBJS_HOST)
	$(GCC_CC) -o $@ $^ $(GCC_LDFLAGS)

#include additional dependencies on object file (e.g. header files)
-include $(TEST_GAPP_OBJS_HOST:.o=.d)

#Compile HOST
$(BUILDDIR_HOST)/%.o: %.c
	mkdir -p $(BUILDDIR_HOST)
	mkdir -p $(TARGETDIR_HOST)
	$(GCC_CC) $(GCC_CCFLAGS) -c $< -o $@ -MMD -MF $(BUILDDIR_HOST)/$*.d

#-----------------------------------------------------------------------------------------------------------------------
# CLEAN
#-----------------------------------------------------------------------------------------------------------------------
.PHONY: clean
clean:
	if [ -d "./$(BUILDDIR_HOST)" ]; then rm -rf $(BUILDDIR_HOST)/*; fi
	if [ -d "./$(TARGETDIR_HOST)" ]; then rm -rf $(TARGETDIR_HOST)/*;	fi
	rm $(TEST_SOURCES)