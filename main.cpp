#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>

struct WordCount
{
	WordCount() = default;
	WordCount(const WordCount &other) = default;
	WordCount &operator=(const WordCount &other) = default;

	int charCount = 0;
	int wordCount = 0;
	int lineCount = 0;
};

WordCount operator+ (const WordCount &lhs, const WordCount &rhs)
{
	WordCount result = lhs;
	result.charCount += rhs.charCount;
	result.wordCount += rhs.wordCount;
	result.lineCount += rhs.lineCount;
	return result;
}

struct ParsedArguments
{
	ParsedArguments() = default;
	ParsedArguments(const ParsedArguments &other) = default;
	ParsedArguments &operator=(const ParsedArguments &other) = default;

	enum Flags {
		NoFlags = 0,
		CharCount = 0x1,
		WordCount = 0x2,
		LineCount = 0x4,
		DefaultMask = 0x7,
		Help = 0x8,
	};

	std::vector<std::string> files;
	int flags = NoFlags;
};

WordCount handleStream(std::istream &input_stream)
{
	WordCount result;
	char c = 0;

	do {
		while (input_stream.get(c)) {
			if (isalpha(c)) {
				result.wordCount++;
				break;
			}

			result.charCount++;
			if (c == '\n')
				result.lineCount++;
		}

		while (input_stream) {
			result.charCount++;
			if (c == '\n')
				result.lineCount++;

			if (isspace(c) || c == '\n') {
				break;
			}
			input_stream.get(c);
		};

	} while (input_stream);

	return result;
}

WordCount handleFile(const std::string &fileName)
{
	std::ifstream file;
	file.open(fileName);
	return handleStream(file);
}

ParsedArguments parseArguments(int argc, char *argv[])
{
	ParsedArguments result;

	int opt = 0;
	while ((opt = getopt(argc, argv, "mlwh")) != -1) {
		switch (opt) {
		case 'm':
			result.flags |= ParsedArguments::Flags::CharCount;
			break;
		case 'l':
			result.flags |= ParsedArguments::Flags::LineCount;
			break;
		case 'w':
			result.flags |= ParsedArguments::Flags::WordCount;
			break;
		case 'h':
			result.flags |= ParsedArguments::Flags::Help;
			break;
		default:
			break;
		}
	}

	if ((result.flags & ParsedArguments::Flags::DefaultMask) == 0)
		result.flags |= ParsedArguments::Flags::DefaultMask;

	for (int i = optind; i < argc; ++i)
		result.files.push_back(argv[i]);

	return result;
}

std::string joinStrings(const std::vector<std::string> &strings, const std::string &separator = ", ")
{
	std::string result;
	for (auto string : strings) {
		result += string + separator;
	}

	if (!result.empty()) {
		result.resize(result.size() - separator.size());
	}
	return result;
}

std::string formattedString(const WordCount &count, int flags, const std::string &fileName = std::string())
{
	std::vector<std::string> result;

	using Flags = ParsedArguments::Flags;

	if (flags & Flags::LineCount)
		result.push_back(std::to_string(count.lineCount));

	if (flags & Flags::WordCount)
		result.push_back(std::to_string(count.wordCount));

	if (flags & Flags::CharCount)
		result.push_back(std::to_string(count.charCount));

	if (!fileName.empty())
		result.push_back(fileName);

	return joinStrings(result, "\t");
}

void printCount(const WordCount &count, int flags, const std::string &fileName = std::string())
{
	std::cout << formattedString(count, flags, fileName) << std::endl;
}

int main(int argc, char *argv[])
{
	try {

		auto parsed = parseArguments(argc, argv);

		if (parsed.flags & ParsedArguments::Flags::Help) {
			std::cout << "Usage: " << "mywc" << " [-m] [-l] [-w] [FILE]..." << std::endl;
			return 1;
		}

		if (parsed.files.empty()) {
			auto count = handleStream(std::cin);
			std::cout << formattedString(count, parsed.flags) << std::endl;
		} else if (parsed.files.size() == 1) {
			const auto fileName = parsed.files.front();
			auto count = handleFile(fileName);
			std::cout << formattedString(count, parsed.flags, fileName) << std::endl;
		} else {
			WordCount totalCount;
			for (auto fileName : parsed.files) {
				const auto count = handleFile(fileName);
				totalCount = totalCount + count;

				std::cout << formattedString(count, parsed.flags, fileName) << std::endl;
			}

			std::cout << formattedString(totalCount, parsed.flags, "total") << std::endl;
		}

	} catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << "Caught unknown exception" << std::endl;
		return -1;
	}

	return 0;
}
