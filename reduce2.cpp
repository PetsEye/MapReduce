#include <sstream>
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <thread>

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
            std::string clean{};
            for (auto c : word)
            {
                if(std::isalpha(c))
                {
                    clean += std::tolower(c);
                }
            }
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

int main() {
    std::string filename = "divineComedy.txt";

    std::vector<std::pair<std::string, int>> final_output = group_and_reduce(map(filename));

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
