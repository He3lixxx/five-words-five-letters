#include <vector>
#include <unordered_map>
#include <bitset>
#include <bit>
#include <cstdint>
#include <iostream>
#include <string>
#include <algorithm>

std::vector<uint32_t> words;
std::unordered_map<uint32_t, std::vector<std::string>> word_mappings;

std::bitset<1 << 26> tried_without_success;

std::vector<uint32_t> current_words_stack;

uint64_t result_count = 0;
uint64_t result_with_anagram_count = 0;

bool recurse(uint32_t chars_so_far, int words_so_far, size_t start_index) {
    if (words_so_far == 5) {
        uint64_t anagram_combinations = 1;
        std::cout << "Found solution for 5 words: ";
        for (uint32_t word : current_words_stack) {
            std::cout << "[";
            anagram_combinations *= word_mappings[word].size();
            for (const auto& str : word_mappings[word]) {
                std::cout << str << ",";
            }
            std::cout << "], ";
        }
        std::cout << "\n";

        result_count += 1;
        result_with_anagram_count += anagram_combinations;

        return true;
    }

    // don't descend into the same starting characters multiple times
    if (words_so_far >= 2) {
        if (tried_without_success[chars_so_far]) {
            return false;
        }
    }

    bool success = false;
    for (size_t i = start_index; i < words.size(); ++i) {
        uint32_t word = words[i];

        if ((word & chars_so_far) == 0) {
            current_words_stack.push_back(word);
            success |= recurse(word | chars_so_far, words_so_far + 1, i + 1);
            current_words_stack.pop_back();
        }
    }

    if(!success) {
        tried_without_success[chars_so_far] = true;
    }

    return success;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    constexpr size_t expected_unique_five_letter_word_count = 5977;

    word_mappings.reserve(expected_unique_five_letter_word_count);
    current_words_stack.reserve(5);

    size_t five_letter_word_count = 0;

    auto str_word = std::string("");
    while (std::cin >> str_word) {
        if (str_word.length() != 5) {
            continue;
        }

        ++five_letter_word_count;

        uint32_t word = 0;
        for (unsigned char c : str_word) {
            word |= 1 << (std::tolower(c) - 'a');
        }

        if (std::popcount(word) != 5) {
            continue; // contains a character multiple times.
        }

        word_mappings[word].push_back(str_word);
    }

    words.resize(word_mappings.size());
    std::transform(word_mappings.begin(), word_mappings.end(), words.begin(), [](const auto& el) {return el.first;});

    std::cerr << "Read " << five_letter_word_count << " words with 5 characters.\n";
    std::cerr << "of those, " << word_mappings.size() << " unique words remain when removing anagrams.\n";

    std::sort(words.begin(), words.end());  // for cache efficiency with the bitset.

    recurse(0, 0, 0);

    std::cerr << "Done. " << result_count << " results found (equals " << result_with_anagram_count
        << " results when including anagrams).\n";
}
