TEST_SOURCE := $(patsubst %,%.cpp,$(TEST_SUITE))
TEST_LIB := -lgtest_main -lgtest

TEST_trie := test_trie

# .PHONY: $(TEST_trie)
.PHONY: test-build-env

$(TEST_trie): test_trie

test_trie: ss_trie.o
	g++ -g -O0 -I. -Igtest/include -Lgtest/build -o $@ test/$@.cpp $(TEST_LIB) $^

TEST_SUITE += test/test_trie test/test_trie

# $(TEST_SUITE):
# 	@./$@
