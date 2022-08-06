#include <vector>
#include <unordered_map>
#include <bitset>
#include <bit>
#include <cstdint>
#include <iostream>
#include <string>
#include <algorithm>

static std::vector<uint32_t> words;
static std::unordered_map<uint32_t, std::vector<std::string>> word_mappings;
static std::vector<uint32_t> tried_without_success_from(1<<26, -1);
static std::bitset<1<<26> tried_without_success_from_bloom_filter;

static uint32_t current_words_stack[5];

static uint64_t result_count = 0;
static uint64_t result_with_anagram_count = 0;

static bool recurse(uint32_t chars_so_far, int words_so_far, uint32_t start_index) {
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

    auto words_begin = words.begin();
    auto words_end = words.end();
    auto word_it = words_begin + start_index;

    // don't descend into the same starting characters multiple times
    if (words_so_far >= 2 && tried_without_success_from_bloom_filter[chars_so_far]) {
        uint32_t previously_tested_from = tried_without_success_from[chars_so_far];
        if (previously_tested_from <= start_index) {
            return false;
        }
        if (previously_tested_from != -1) {
            words_end = words_begin + previously_tested_from;
        }
    }

    bool success = false;
    while (true) {
        word_it = std::find_if(word_it, words_end, [&](uint32_t word){ return (word & chars_so_far) == 0; });
        if (word_it == words_end) {
            break;
        }

        uint32_t word = *word_it;
        current_words_stack[words_so_far] = word;
        success |= recurse(word | chars_so_far, words_so_far + 1, word_it - words_begin + 1);
        ++word_it;
    }

    if (!success && words_so_far <= 3) {
        tried_without_success_from_bloom_filter[chars_so_far] = true;
        tried_without_success_from[chars_so_far] = start_index;
    }

    return success;
}

uint32_t bit_for_char(unsigned char c) {
    switch(c) {
        case 'e': return 1 << 0;
        case 'a': return 1 << 1;
        case 'r': return 1 << 2;
        case 'i': return 1 << 3;
        case 'o': return 1 << 4;
        case 't': return 1 << 5;
        case 'n': return 1 << 6;
        case 's': return 1 << 7;
        case 'l': return 1 << 8;
        case 'c': return 1 << 9;
        case 'u': return 1 << 10;
        case 'd': return 1 << 11;
        case 'p': return 1 << 12;
        case 'm': return 1 << 13;
        case 'h': return 1 << 14;
        case 'g': return 1 << 15;
        case 'b': return 1 << 16;
        case 'f': return 1 << 17;
        case 'y': return 1 << 18;
        case 'w': return 1 << 19;
        case 'k': return 1 << 20;
        case 'v': return 1 << 21;
        case 'x': return 1 << 22;
        case 'z': return 1 << 23;
        case 'j': return 1 << 24;
        case 'q': return 1 << 25;
        default:
            throw std::runtime_error("Invalid input character: " + std::to_string(c));
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    constexpr size_t expected_unique_five_letter_word_count = 5977;
    word_mappings.reserve(expected_unique_five_letter_word_count);

    size_t five_letter_word_count = 0;

    auto str_word = std::string("");
    while (std::cin >> str_word) {
        if (str_word.length() != 5) {
            continue;
        }
        ++five_letter_word_count;

        uint32_t word = 0;
        for (unsigned char c : str_word) {
            word |= bit_for_char(c);
        }

        if (std::popcount(word) != 5) {
            continue;  // contains a character multiple times.
        }
        word_mappings[word].push_back(str_word);
    }
    words.resize(word_mappings.size());
    std::transform(word_mappings.begin(), word_mappings.end(), words.begin(), [](const auto& el) {return el.first;});

    std::cerr << "Read " << five_letter_word_count << " words with 5 characters.\n";
    std::cerr << "of those, " << word_mappings.size() << " unique words remain when removing anagrams.\n";

    std::sort(words.begin(), words.end());  // for cache efficiency

    recurse(0, 0, 0);

    std::cerr << "Done. " << result_count << " results found (equals " << result_with_anagram_count
        << " results when including anagrams).\n";
}
