# 
# Add unit test here using Google C++ Testing Framework
# 
# Follow the example to add more unit tests

test_trie: test/test_trie.cpp obj/ss_trie.o
	$(SS_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

TEST_SUITE := test_trie
