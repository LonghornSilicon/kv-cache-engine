CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -fPIC
PYTHON   ?= python3

SRC      := kv_cache_engine_ref.cpp
HDR      := kv_cache_engine_ref.hpp
TEST_SRC := test_kv_cache_engine_ref.cpp
TEST_BIN := test_kv
LIB_SO   := libkv_cache_engine_ref.so
LIB_A    := libkv_cache_engine_ref.a

# ChannelQuant reference (the revamped codec) — C++ leg of 3-way parity.
CQ_SRC      := channelquant_ref.cpp
CQ_HDR      := channelquant_ref.hpp
CQ_TEST_SRC := test_channelquant_ref.cpp
CQ_TEST_BIN := test_cq

# RHT sign-register test (boot-loaded value sign vector) — no vendored vectors.
SR_TEST_SRC := test_sign_register.cpp
SR_TEST_BIN := test_signreg

TV_DIR   := ../../rtl/tb/testvectors

.PHONY: all test test-py test-cq test-signreg test-all shared static clean testvectors

all: $(TEST_BIN) $(CQ_TEST_BIN) $(SR_TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) $(SRC) -lm

test: $(TEST_BIN) testvectors
	./$(TEST_BIN)

# ChannelQuant golden-vector parity (contract §8). Needs the vendored vectors in
# $(TV_DIR)/channelquant/hex — committed, no generation step.
$(CQ_TEST_BIN): $(CQ_TEST_SRC) $(CQ_SRC) $(CQ_HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(CQ_TEST_SRC) $(CQ_SRC) -lm

test-cq: $(CQ_TEST_BIN)
	./$(CQ_TEST_BIN)

$(SR_TEST_BIN): $(SR_TEST_SRC) $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(SR_TEST_SRC) $(SRC) -lm

test-signreg: $(SR_TEST_BIN)
	./$(SR_TEST_BIN)

test-py: testvectors
	$(PYTHON) test_kv_cache_engine_ref.py

test-all: test test-cq test-signreg test-py

shared: $(LIB_SO)

$(LIB_SO): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(SRC) -lm

static: $(LIB_A)

$(LIB_A): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -c -o kv_cache_engine_ref.o $(SRC)
	ar rcs $@ kv_cache_engine_ref.o

testvectors:
	@if [ ! -f $(TV_DIR)/input_vectors.hex ]; then \
		echo "Generating test vectors..."; \
		$(PYTHON) ../../analysis/gen_kv_testvectors.py; \
	fi

clean:
	rm -f $(TEST_BIN) $(CQ_TEST_BIN) $(SR_TEST_BIN) $(LIB_SO) $(LIB_A) *.o
