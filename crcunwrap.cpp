/*
 * Harpocrates Paper Backup & Restore Tool
 *
 * Copyright 2019 Hartle & Klug Consulting GmbH
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
		TCLAP::CmdLine cmd("crcunwrap checks and removes continuous CRC16-ARC checksums from binary data previously wrapped with crcwrap.", ' ', "1.0");
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
		if (outputFile == "-")
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
		std::array<char, 16> input_buffer;
		std::array<char, 14> output_buffer;
		std::uint16_t old_crc = 0;
		std::uint16_t *parsed_crc = nullptr;
		int row = 0;

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
			std::uint16_t crc = CRC::Calculate(input_buffer.data() + 2, len - 2, CRC::CRC_16_ARC(), old_crc);
			old_crc = crc;

			parsed_crc = (std::uint16_t *)input_buffer.data();
			if(*parsed_crc != crc)
			{
				std::ostringstream errorMessage;
				errorMessage << "CRC error in row " << (row + 1) << " (address " << std::setfill('0') << std::setw(4) << std::hex << (row * 16) << "); read CRC " << std::hex << std::setfill('0') << std::setw(2) << (*parsed_crc & 0xFF) << ((*parsed_crc >> 8) & 0xFF) << ", but data yields CRC " << std::hex << std::setfill('0') << std::setw(2) << (crc & 0xFF) << ((crc >> 8) & 0xFF) << "."; 
				throw std::runtime_error(errorMessage.str());
			}

			// Write block
			std::fwrite(input_buffer.data() + 2, 1, len - 2, outputStream);

			row++;
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
