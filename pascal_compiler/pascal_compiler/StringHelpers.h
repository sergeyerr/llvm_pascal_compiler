#pragma once
#include <string>
#include <fstream>
#include <vector>

std::vector<std::string> ReadLinesFromFile(std::string filename);
void DeleteLineComments(std::vector<std::string>& lines);
std::string MergeLines(std::vector<std::string> lines);
void DeleteBlockComments(std::string& program);
