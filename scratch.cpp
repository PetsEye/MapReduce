#include <iostream>
#include <fstream>
#include <cctype>

int main()
{
    std::ifstream file("divineComedy.txt");
    file.seekg(1000);
    

    std::string word;
    char ch;

    while (file.get(ch)) {
        if (std::isspace(ch)) {
            break; 
        }
        word += ch;
    }

    std::cout << word << std::endl;
    std::cout << "ended at offset: " << file.tellg() << std::endl;
}
