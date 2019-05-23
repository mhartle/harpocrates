#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <CRCpp/CRC.h>
#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{
	try
	{
		TCLAP::CmdLine cmd("crcwrap inserts continuous CRC16-ARC checksums into binary data.", ' ', "1.0");
		TCLAP::ValueArg<std::string> inputArg("i", "input", "File to read from, \"-\" is used by default for stdin", false, "-", "string");
		TCLAP::ValueArg<std::string> outputArg("o", "output", "File to write to, use \"-\" for stdout", true, "-", "string");
		cmd.add(outputArg);
		cmd.add(inputArg);

		// Parse command line
		cmd.parse(argc, argv);

		std::string inputFile = inputArg.getValue();
		std::FILE* inputStream = nullptr;
		std::string outputFile = outputArg.getValue();
		std::FILE* outputStream = nullptr;

		// Handle stdin as input
		if(inputFile == "-")
		{
			inputStream = stdin;
			std::freopen(nullptr, "rb", inputStream);
		}
		else
		{
			inputStream = std::fopen(inputFile.data(), "rb");

			if(inputStream == nullptr)
			{
				std::cerr << "Failed to open " << inputFile << " for reading." << std::endl;
				return -1;
			}
		}

		if(std::ferror(inputStream))
		{
			throw std::runtime_error(std::strerror(errno));
		}

		// Handle stdout as input
		if(outputFile == "-")
		{
			outputStream = stdout;
			std::freopen(nullptr, "wb", outputStream);
		}
		else
		{
			outputStream = std::fopen(outputFile.data(), "wb");

			if(outputStream == nullptr)
			{
				std::cerr << "Failed to open " << outputFile << " for writing." << std::endl;
				return -1;
			}
		}

		if(std::ferror(outputStream))
		{
			throw std::runtime_error(std::strerror(errno));
		}

		std::size_t i;
		std::size_t len;
		std::array<char, 14> input_buffer;
		std::array<char, 16> output_buffer;
		std::uint16_t old_crc = 0;

		// std::cout << "Wrapping " << inputFile << " to " << outputFile << "..." << std::endl;

		// use std::fread and remember to only use as many bytes as are returned
		// according to len
		while((len = std::fread(input_buffer.data(), sizeof(input_buffer[0]), input_buffer.size(), inputStream)) > 0)
		{
			// Handle stdin EOF and errors
			if(std::ferror(stdin) && !std::feof(stdin))
			{
				throw std::runtime_error(std::strerror(errno));
			}

			// Compute a continuous CRC for the new block
			std::uint16_t crc = CRC::Calculate(input_buffer.data(), len, CRC::CRC_16_ARC(), old_crc);
			old_crc = crc;

			// Write CRC-protected block
			std::fwrite((const void*)&crc, sizeof(std::uint16_t), 1, outputStream);
			std::fwrite(input_buffer.data(), 1, len, outputStream);
		}

		if(inputStream != stdin)
		{
			std::fclose(inputStream);
		}

		if(outputStream != stdout)
		{
			std::fclose(outputStream);
		}
	}
	catch(std::exception const& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
}
