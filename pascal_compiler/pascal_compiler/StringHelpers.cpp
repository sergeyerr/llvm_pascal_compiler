#include "StringHelpers.h"
#include <sstream>

std::vector<std::string> ReadLinesFromFile(std::string filename) {
	std::ifstream infile(filename);
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(infile, line))
		lines.push_back(line + '\n');
	return lines;
}


void DeleteLineComments(std::vector<std::string> &lines) {
	for (auto line : lines)
	{
		bool string_const = false;
		bool comment = false;
		bool first_slash = false;
		for (int i = 0; i < line.length(); i++) {
			char ch = line[i];
			if (ch == '\'') 
				string_const = !string_const && !comment;
			if (string_const) continue;
			if (comment) {
				if (ch != '\n') line[i] = ' ';
			}
			else {
				if (ch == '/') {
					comment = first_slash;
					first_slash = true;
					// удаляем текущий и предыдущий слеш
					if (comment) {
						line[i] = ' ';
						line[i-1] = ' ';
					}
				}
			}
		}
	}
}


std::string MergeLines(std::vector<std::string> lines) {
	std::string res;
	for (auto line : lines) {
		res += line;
	}
	return res;
}


void DeleteBlockComments(std::string& program) {
	bool comment = false;
	bool string_const = false;
	for (int i = 0; i < program.length(); i++) {
		char ch = program[i];
		if (ch == '\'')
			string_const = !string_const && !comment;
		if (string_const) continue;
		if (comment) {
			if (ch == '}') comment = false;
			if (ch != '\n') program[i] = ' ';
		}
		else if (ch == '{') {
			comment = true;
			program[i] = ' ';
		}
	}
}