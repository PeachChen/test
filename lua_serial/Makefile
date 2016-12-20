#x:=xx 这种赋值 避免死循环

#编译链接 默认宏
CC:=gcc
CXX:=g++
#打包程序
AR=ar
#打包参数
ARFLAGS=rucs
#默认编译C文件命令宏
CFLAGS:=-Wall -g -m64
#默认编译CPP文件命令宏
CXXFLAGS:=-Wall -g -m64
#包含文件
INCLUDE:=-I. -I../public
#包含库
LIBS:=-llua -ldl
#包含库的目录
LIBPATH:=
#预定义
MARCO:=
#要编译的文件后缀
CEXTS=.c
CCEXTS=.cpp
SRCEXTS:=$(CEXTS) $(CCEXTS)
#目标
TARGET:=./tlua
#obj目录
OBJPATH:=./obj

LDFLAGS=$(LIBPATH) $(LIBS)
CPPFLAGS:=$(INCLUDE) $(MARCO)
COMPILE.CC=$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH)
COMPILE.C=$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH)
LINK.CC=$(CXX) $(OBJS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) -o $(TARGET)
LINK.C=$(CC) $(OBJS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) -o $(TARGET)
LINK.AR=$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
PREPROC.CC=$(CXX) $(CXXFLAGS) $(CPPFLAGS)
PREPROC.C=$(CC) $(CFLAGS) $(CPPFLAGS)

# 临时目录不存在时自动创建
ifeq (,$(filter $(MAKECMDGOALS),clean list))
$(shell mkdir -p $(OBJPATH))
endif
# 设置源代码所在目录，指定目录下所有子目录中的文件都将被加入项目中，若对某些目录或文件忽略需要特别指定;
# 指定目录时可使用相对路径（相对Makefile的路径），多目录可以使用空格隔开
# TODO 注意：源文件和目录名不能含有' '、'$'、'#'、'@'、'?'、'%'、'~'等特殊字符
SRCDIRS := ./
# 某些不再指定源代码目录中的文件可以额外添加
ADDITION:=../public/util/MyStream.cpp

# 可以将源代码目录中指定的子目录忽略，从而从项目中排除
EXCLUDE_DIRS:=
# 忽略的文件，即不作为项目源文件，而又不在指定忽略目录中的文件
EXCLUDE_FILES:=

# 自动查找所有源文件
FILES:= $(shell \
		excludes=; \
		for dir in $(EXCLUDE_DIRS); do\
			excludes=$$excludes' '`find $$dir -type f -iregex ".*\.\(c\|cc\|cpp\|cxx\)"`;\
		done;\
		for file in $(EXCLUDE_FILES); do\
			excludes=$$excludes' '$$file;\
		done;\
		for dir in $(SRCDIRS); do\
			if [ -d $$dir ]; then\
				filelist=`find $$dir -type f -iregex ".*\.\(c\|cc\|cpp\|cxx\)"`;\
				for file in $$filelist; do\
					for entry in $$excludes; do\
						if [ "$$file" -ef "$$entry" ]; then\
							continue 2;\
						fi;\
					done;\
					if [ -f $$file ]; then\
						file=`echo $$file | sed "s/^\.\///"`;\
						printf "%s " $$file;\
					fi;\
				done;\
			fi;\
		done;\
		for file in $(ADDITION); do\
			if [ -f $$file ]; then\
				file=`echo $$file | sed "s/^\.\///"`;\
				printf "%s " $$file;\
			fi;\
		done;)

# 为了能从目标文件得到源文件路径，将全路径中的'/'替换成了'~'，'.'替换成了'@'
# 因此看到有~和@符号的.o文件请不用惊讶
OBJS    := $(addprefix $(OBJPATH)/,$(addsuffix .o,$(shell echo $(FILES) | sed "s/\//\@/g" | sed "s/\./~/g")))

# 根据目标文件反向得到源文件的全路径
obj2name=$(shell echo $* | sed "s/\@/\//g" | sed "s/~/\./g")

ifeq ($(filter-out $(addprefix %,$(CEXTS)),$(FILES)),)
LINK=$(LINK.C)
else
LINK=$(LINK.CC)
endif

ifeq ($(suffix $(TARGET)),.a)
LINK=$(LINK.AR)
endif

.PHONY:list all clean

all: $(TARGET)
	@mkdir -p $(<D)
	$(LINK)
$(TARGET):$(OBJS)
list:
	@echo $(FILES)
	@echo $(OBJS)
$(OBJPATH)/%.o: $(obj2name) $(OBJPATH)/%.d
	@if [ $(CEXTS) == $(suffix $(obj2name)) ];then\
		$(COMPILE.C) -c $(obj2name) -o $@; \
		echo '$(COMPILE.C) -c $(obj2name) -o $@'; \
	else \
		$(COMPILE.CC) -c $(obj2name) -o $@; \
		echo '$(COMPILE.CC) -c $(obj2name) -o $@'; \
	fi

ifeq (,$(filter $(MAKECMDGOALS),clean list))
$(OBJPATH)/%.d:$(obj2name)
	@if [ $(CEXTS) == $(suffix $(obj2name)) ];then\
		$(PREPROC.C) -MM $(obj2name) -MT$(@:.d=.o) -MF$@; \
		echo '$(PREPROC.C) -MM $(obj2name) -MT$(@:.d=.o) -MF$@'; \
	else \
		$(PREPROC.CC) -MM $(obj2name) -MT$(@:.d=.o) -MF$@; \
		echo '$(PREPROC.CC) -MM $(obj2name) -MT$(@:.d=.o) -MF$@'; \
	fi

#文件包含 包含每一个.d文件
-include $(OBJS:.o=.d)
endif
		

clean:
	-rm -rf $(TARGET)
	-rm -rf $(OBJPATH)
