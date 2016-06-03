#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>

struct WordCount
{
	WordCount() = default;
	WordCount(const WordCount &other) = default;
	WordCount &operator=(const WordCount &other) = default;

	int chars = 0;
	int words = 0;
	int lines = 0;
};

WordCount operator+ (const WordCount &lhs, const WordCount &rhs)
{
	WordCount result = lhs;
	result.chars += rhs.chars;
	result.words += rhs.words;
	result.lines += rhs.lines;
	return result;
}

struct ParsedArguments
{
	std::vector<std::string> files;
};

WordCount handleStream(std::istream &input_stream)
{
	WordCount result;
	char c = 0;

	while (input_stream.get(c)) {
		if (c == ' ')
			result.words++;
		else if (c == '\n')
			result.words++;
		else
			result.chars++;
	}

	return result;
}

ParsedArguments parse(int argc, char *argv[])
{
	ParsedArguments result;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			result.files.push_back(argv[i]);
	}

	return result;
}

int main(int argc, char *argv[])
{
	try {
		auto parsed = parse(argc, argv);

		WordCount totalCount;

		if (parsed.files.empty()) {
			totalCount = handleStream(std::cin);
		} else {
			for (auto fileName : parsed.files) {
				std::ifstream file;
				file.open(fileName);
				totalCount = totalCount + handleStream(file);
			}
		}

		std::cout << totalCount.lines << "\t" << totalCount.words << "\t" << totalCount.chars << std::endl;

	} catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << "Caught unknown exception" << std::endl;
		return -1;
	}

	return 0;
}
