# Marcus Schmidt

CXXFLAGS = -Wall -std=c++17

wxPath = ~/Library/wxWidgets-3.2.1/build-cocoa-debug
wxComp = `$(wxPath)/wx-config --cxxflags`
wxLink = `$(wxPath)/wx-config --libs`

obj_dir = obj
obj_path := ./$(obj_dir)

src_path = src
src_files := $(wildcard $(src_path)/*.cpp) # get all .cpp files in source directory

# The variable 'obj_files' is evaluated to contain all the .o (object) files that are compiled from the source code.
# This is done by swapping the extension from .cpp to .o, which looks at one of the rules below to
# properly compile it with the wxWidgets flags.
obj_files := $(src_files:$(src_path)/%.cpp=$(obj_path)/%.o) # $(pathsubst $(src_path)/%.cpp,$(obj_path)/%.o,$(src_files))

# The primary rule links all of the object files together and creates the executable.
TotalPhoto.out: $(obj_files)
	$(CXX) $^ $(wxLink) -o $@

# This rule provides the template for compiling a single .o file from a .cpp file.
# If the directory for object files doesn't already exist, it makes one.
$(obj_path)/%.o: $(src_path)/%.cpp | $(obj_dir)
	$(CXX) $(CXXFLAGS) -c $< $(wxComp) -o $@

$(obj_dir):
	mkdir $(obj_dir)

.PHONY: clean clean_all

clean:
	rm $(obj_path)/*.o
	rmdir $(obj_dir)

clean_all: clean
	rm *.out
