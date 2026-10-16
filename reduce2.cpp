#include <sstream>
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <thread>

std::vector<std::pair<std::string, int>> map_chunk(const std::string& filename, std::streamoff start, std::streamoff end);
std::string clean_word(const std::string&);
std::streamoff snap_to_whitespace(std::ifstream& file, std::streamoff target, std::streamoff fileSize);

std::vector<std::pair<std::string, int>> map(const std::string& filename)
{
    std::vector<std::pair<std::string, int>> results;
    std::ifstream file(filename);
    
    std::string line;
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::string word;
        
        while (ss >> word)
        {   
            std::string clean = clean_word(word);
            if (!clean.empty())
            {
                results.push_back({clean, 1});
            }
        }
    }
    
    return results;
}

std::pair<std::string, int> reduce(const std::string& key, const std::vector<int>& values) 
{
    int sum = 0;
    for (int v : values) {
        sum += v; 
    }
    return {key, sum};
}

std::vector<std::pair<std::string, int>> group_and_reduce(std::vector<std::pair<std::string, int>> pairs)
{
    std::sort(pairs.begin(), pairs.end());

    std::vector<std::pair<std::string, int>> final_output;

    if (pairs.empty()) return final_output;

    std::string current_key = pairs[0].first;
    std::vector<int> current_values;

    for (const auto& pair : pairs) {
        if (pair.first == current_key) {
            current_values.push_back(pair.second);
        } else {
            final_output.push_back(reduce(current_key, current_values));

            current_key = pair.first;
            current_values.clear();
            current_values.push_back(pair.second);
        }
    }

    final_output.push_back(reduce(current_key, current_values));

    return final_output;
}

std::vector<std::streamoff> compute_boundaries(const std::string& filename, int k)
{
    std::ifstream file(filename);
    
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();

    std::vector<std::streamoff> split_points;
    split_points.reserve(k + 1); // pre-allocating memory.

    for (int i = 0; i <= k; ++i) 
    {
        auto target = static_cast<std::streamoff>(i) * fileSize / k; //static casting cuz i is a 32 bit int while streamoff is 64.
        std::streamoff adjusted = snap_to_whitespace(file, target, fileSize);
        split_points.push_back(adjusted);
    }
    
    return split_points;
}

std::streamoff snap_to_whitespace(std::ifstream& file, std::streamoff target, std::streamoff fileSize)
{
    if (target <= 0) 
    {
        return 0;
    }
    if (target >= fileSize) 
    {
        return fileSize;
    }

    file.seekg(target);
    
    int c;
   
    while ((c = file.peek()) != std::ifstream::traits_type::eof() &&
           !std::isspace(static_cast<unsigned char>(c))) 
    {
        file.get();
    }

    std::streamoff adjusted = file.tellg();

    
    if (adjusted == -1) {
        file.clear();
        return fileSize;
    }

    return std::min(adjusted, fileSize);
}

std::string clean_word(const std::string& word)
{
    std::string clean{};
    for (auto c : word)
    {
        if(std::isalpha(c))
        {
            clean += std::tolower(c);
        }
    }
    return clean;
}

std::vector<std::pair<std::string, int>> map_chunk(const std::string& filename, std::streamoff start, std::streamoff end)
{
    std::vector<std::pair<std::string, int>> results;
    if (end <= start) return results;

    std::ifstream file(filename);
    file.seekg(start);

    std::string buffer(end - start, '\0');
    file.read(&buffer[0], end - start);
    buffer.resize(static_cast<size_t>(file.gcount()));

    size_t i = 0;
    while (i < buffer.size()) {
        while (i < buffer.size() && std::isspace(static_cast<unsigned char>(buffer[i]))) {
            ++i;
        }
        size_t word_start = i;
        while (i < buffer.size() && !std::isspace(static_cast<unsigned char>(buffer[i]))) {
            ++i;
        }
        if (i > word_start) {
            std::string cleaned = clean_word(buffer.substr(word_start, i - word_start));
            if (!cleaned.empty()) {
                results.emplace_back(std::move(cleaned), 1);
            }
        }
    }

    return results;
}

std::vector<std::pair<std::string, int>> map_parallel(const std::string& filename, int k)
{
    std::vector<std::streamoff> bounds = compute_boundaries(filename, k);

    std::vector<std::vector<std::pair<std::string, int>>> slots(k);
    std::vector<std::thread> threads;

    for (int i = 0; i < k; ++i)
    {
        threads.emplace_back([&filename, &bounds, &slots, i]() {
            slots[i] = map_chunk(filename, bounds[i], bounds[i + 1]);
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::vector<std::pair<std::string, int>> all;
    for (const auto& s : slots)
    {
        all.insert(all.end(), s.begin(), s.end());
    }
    return all;
}

int main() {
    std::string filename = "divineComedy.txt";

    std::vector<std::pair<std::string, int>> final_output = group_and_reduce(map_parallel(filename, 4));

    // Sort final results by highest frequency 
    std::sort(final_output.begin(), final_output.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });

    std::cout << "--- Top 20 Most Frequent Words in Divine Comedy ---\n";
    size_t limit = std::min(size_t(20), final_output.size());
    for (size_t i = 0; i < limit; ++i) {
        std::cout << i + 1 << ". " << final_output[i].first << " -> " << final_output[i].second << "\n";
    }


    return 0;
}
