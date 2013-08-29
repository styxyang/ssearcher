# 
# Add unit test here using Google C++ Testing Framework
# 
# Follow the example to add more unit tests

test_trie: test/test_trie.cpp obj/trie.o
	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

test_buf: test/test_buf.cpp obj/buffer.o
	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

test_match: test/test_match.cpp obj/match.o
	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

TEST_SUITE := test_trie test_buf test_match
