# assumes GCC-compatible compiler.
five_words: five_words.cpp
	$(CXX) -std=c++20 -O3 -march=native -o five_words five_words.cpp
