GXX = g++
INCFLAGS = -I fmt/include
CXX_FFAGS = -Wall -Wextra -std=c++17 -O2
SOURCE_DIR = src
TEST_DIR = test

all:
	$(GXX) $(CXX_FFAGS) $(INCFLAGS) -o external_sort_test.exe $(SOURCE_DIR)/test.cpp

clean:
	rm -f $(TEST_DIR)/*.sorted external_sort_test.exe