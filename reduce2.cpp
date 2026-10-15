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

void run_shuffle_and_reduce(std::vector<std::pair<std::string, int>>& map_results) {
    
    std::sort(map_results.begin(), map_results.end());

    std::vector<std::pair<std::string, int>> final_output;

    if (map_results.empty()) return;

    
    std::string current_key = map_results[0].first;
    std::vector<int> current_values;

    
    for (const auto& pair : map_results) {
        if (pair.first == current_key) {
            current_values.push_back(pair.second);
        } else {
            std::pair<std::string, int> reduced_result = reduce(current_key, current_values);
            final_output.push_back(reduced_result);

            
            current_key = pair.first;
            current_values.clear(); 
            current_values.push_back(pair.second); 
        }
    }

    
    std::pair<std::string, int> last_result = reduce(current_key, current_values);
    final_output.push_back(last_result);

    
    std::cout << "\n--- Final Reduced Word Counts ---\n";
    for (const auto& item : final_output) {
        std::cout << item.first << ": " << item.second << "\n";
    }
}

int main() {
    std::string filename = "divineComedy.txt";
     
    std::vector<std::pair<std::string, int>> map_results = map(filename);
    
    std::sort(map_results.begin(), map_results.end());
    
    std::vector<std::pair<std::string, int>> final_output;

    if (!map_results.empty()) {
        std::string current_key = map_results[0].first;
        std::vector<int> current_values;

        for (const auto& pair : map_results) {
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
    }

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
