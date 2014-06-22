OBJS = benchmark.o bitbase.o bitboard.o book.o endgame.o evaluate.o nacl.o \
	material.o misc.o movegen.o movepick.o notation.o pawns.o position.o \
	search.o thread.o timeman.o tt.o uci.o ucioption.o

HEADERS = $(wildcard ../src/*.h)

NACL_SDK_ROOT ?= $(abspath $(HOME)/sdks/nacl_sdk/pepper_35)

PNACL_TC_PATH = $(NACL_SDK_ROOT)/toolchain/linux_pnacl
PNACL_CXX = $(PNACL_TC_PATH)/bin/pnacl-clang++
PNACL_FINALIZE = $(PNACL_TC_PATH)/bin/pnacl-finalize

CXXFLAGS = -fno-exceptions -fno-rtti -O3 -I$(NACL_SDK_ROOT)/include -DIS_64BIT -DNDEBUG -DNO_PREFETCH
LDFLAGS = -L$(NACL_SDK_ROOT)/lib/pnacl/Release -O3 -lppapi_cpp -lppapi -lpthread

all: stockfish.pexe

%.o: ../src/%.cpp $(HEADERS) Makefile
	$(PNACL_CXX) -c -o $@ $< $(CXXFLAGS)

stockfish.bc: $(OBJS)
	$(PNACL_CXX) -o $@ $(OBJS) $(LDFLAGS)

stockfish.pexe: stockfish.bc
	$(PNACL_FINALIZE) -o $@ $<

clean:
	rm stockfish.pexe stockfish.bc *.o
