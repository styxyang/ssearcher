# 
# Add unit test here using Google C++ Testing Framework
# 
# Follow the example to add more unit tests

# test_trie: test/test_trie.cpp obj/util.o
# 	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

test_buf: test/test_buf.cpp obj/$(BUILD)/buffer.o
	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

# test_match: test/test_match.cpp obj/match.o
# 	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

test_rope: test/test_rope.cpp obj/$(BUILD)/buffer.o obj/$(BUILD)/util.o
	$(SF_CXX) $(CFLAGS) $(TEST_HEADERS) $(TEST_LD_PATH) -o $@ $^ $(TEST_LDFLAGS)

# TEST_SUITE := test_trie test_buf test_match test_rope
TEST_SUITE := test_buf test_rope
