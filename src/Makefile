appname := TallyPiazzaPollResults
.PHONY: TallyPiazzaPollResults

#Use the gnu compiler on macos (default is clink)
CXX := /usr/local/bin/g++-8
CXXFLAGS := -Wall -g -std=c++11


srcfiles := TallyPiazzaPollResults.cpp

all: $(appname)

$(appname):
	@$(CXX) $(CXXFLAGS) $(srcfiles) -o ../TallyPiazzaPollResults

clean:
	rm -f ../TallyPiazzaPollResults

