SRCS = finite_state_machine.cpp buffer.cpp scanner.cpp file_position.cpp token.cpp string.cpp grammar.cpp parser.cpp type_check.cpp make_code.cpp information.cpp main.cpp
EXEC = foobar

CPPFLAGS = -Iinclude

CFLAGS = -std=c11 -O3 -Wall -pedantic
CXXFLAGS = -std=c++11 -O3 -Wall -pedantic


DEPDIR = .dep
OBJDIR = obj
SRCDIR = src
OUTDIR = bin
$(shell mkdir -p $(DEPDIR) > /dev/null)
$(shell mkdir -p $(OBJDIR) > /dev/null)
$(shell mkdir -p $(SRCDIR) > /dev/null)
$(shell mkdir -p $(OUTDIR) > /dev/null)

OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

.PHONY: clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(OUTDIR)/$(EXEC)

clean:
	$(RM) $(DEPDIR)/*.d $(DEPDIR)/*.Td $(DEPDIR)/*~ $(OBJDIR)/*.o $(OBJDIR)/*~ $(OUTDIR)/*~ $(OUTDIR)/$(EXEC) $(SRCDIR)/*~

$(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cc
$(OBJDIR)/%.o : $(SRCDIR)/%.cc $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cxx
$(OBJDIR)/%.o : $(SRCDIR)/%.cxx $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))
