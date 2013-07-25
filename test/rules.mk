TEST_SOURCE := $(patsubst %,%.cpp,$(TEST_SUITE))
TEST_LIB := -lgtest_main -lgtest

TEST_trie := test_trie

test:
	@mkdir -p gtest/build
	@cd gtest/build; cmake ..; make

$(TEST_trie): ss_trie.o | test
	g++ -g -O0 -I. -Igtest/include -Lgtest/build -o $@ test/$@.cpp $(TEST_LIB) $^
	./$@

TEST_SUITE += $(TEST_trie)
test-all: $(TEST_SUITE)
