include ../benchmarks.mk

TESTNAME = statictreebarrier

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
