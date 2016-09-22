#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "types.h"
#include "util.h"
#include "Decoder.h"
#include "Encoder.h"

#define RED   "\x1B[31m"
#define GREEN "\x1B[32m"
#define CYAN  "\x1B[36m"
#define RESET "\x1B[0m"

#define PRINT_ERROR(msg) std::cerr << RED << msg << RESET << std::endl

void printUsage(const char* progName)
{
	printf(
		"\nUsage:\n"
		"  %s [options] input output\n"
		"  %s -b [options] files ...\n\n"
		"Options:\n"
		"  -r           Raw output without header. Header is added by default.\n"
		"  -p           Print info of input files instead of encoding them.\n"
		"  -h           Print this help information.\n"
		"  -b           Batch mode. Converts all input files, gives them *.bin outputs.\n"
		"  -d <dir>     Directory to output files. Only for batch mode.\n"
		"               Defaults to the directory of inputs.\n"
		"  -c <level>   Quality level for ETC1 compression:\n"
		"                 " CYAN "1" RESET "  - Low quality    (fast)\n"
		"                 " CYAN "2" RESET "  - Medium quality (default)\n"
		"                 " CYAN "3" RESET "  - High quality   (slow)\n"
		"  -i <format>  Input format type. Only needed when input files have no header.\n"
		"               Possible types: " CYAN "rgba8, rgb8, rgba5551, rgb565, rgba4, la8, hilo8,\n"
		"               l8, a8, la4, l4, a4, etc1, etc1a4" RESET ".\n"
		"  -o <format>  Output format type. Can be any of the above input types with some\n"
		"               additional special types:\n"
		"                 " CYAN "png" RESET "        - PNG output, disabling alpha channel if not used.\n"
		"                 " CYAN "auto-etc1" RESET "  - ETC1 when input has no alpha, otherwise ETC1A4.\n"
		"                 " CYAN "auto-l8" RESET "    - L8 when input has no alpha, otherwise LA8.\n"
		"                 " CYAN "auto-l4" RESET "    - L4 when input has no alpha, otherwise LA4.\n"
		, progName, progName);
}

int main(int argc, char **argv)
{
	int opt;
	bool optsInvalid = false;

	// Default options
	bool useHeader = true;
	bool printInfo = false;
	bool batchMode = false;
	std::string outputDir;
	PixelFormat formatInput = DefaultFormat;
	PixelFormat formatOutput = DefaultFormat;
	int etc1quality = 2;
	int threadCount = std::thread::hardware_concurrency();
	if (!threadCount)
		threadCount = 4;

	// Parse options
	while ((opt = getopt(argc, argv, "rphbd:c:i:o:")) != -1)
	{
		 switch (opt)
		 {
			// Use raw output
			case 'r':
				useHeader = false;
				break;
			// Print file info
			case 'p':
				printInfo = true;
				break;
			// Print usage help and exit
			case 'h':
				printUsage(argv[0]);
				return 0;
			// Batch mode enable
			case 'b':
				batchMode = true;
				break;
			// Use different directory for output
			case 'd':
				struct stat st;
				outputDir = optarg;
				if ((stat(outputDir.c_str(), &st) != 0) || !(st.st_mode & S_IFDIR))
				{
					PRINT_ERROR(outputDir << " - Not found or not valid directory.");
					return 1;
				}
				if (outputDir.back() != PATH_SEPARATOR)
					outputDir += PATH_SEPARATOR;
				break;
			// ETC1 compression level
			case 'c':
				try
				{
					etc1quality = std::stoi(optarg, nullptr, 10);
					if (etc1quality < 0 || etc1quality > 3)
						throw;
				}
				catch (int e)
				{
					std::cerr << "Invalid ETC1 quality level. Needs to be between 1 and 3 inclusively." << std::endl;
					return 1;
				}
				break;
			// Define format for input file(s)
			case 'i':
				formatInput = formatFromString(optarg);
				if (formatInput == DefaultFormat ||
					formatInput == AutoETC1)
				{
					std::cerr << "Invalid input format: " << formatInput << std::endl;
					return 1;
				}
				break;
			// Define format for output file(s)
			case 'o':
				formatOutput = formatFromString(optarg);
				if (formatOutput == DefaultFormat)
				{
					std::cerr << "Invalid output format: " << formatOutput << std::endl;
					return 1;
				}
				break;
			// Unknown option
			case '?':
				optsInvalid = true;
				break;
			default:
				return 1;
		}
	}
	
	int fileArgCount = argc - optind;

	// Need at least 1 file arg for input
	if (fileArgCount < 1)
	{
		PRINT_ERROR("No input file specified.");
		optsInvalid = true;
	}
	
	// If not in batch mode, need second arg for output
	if (!batchMode && fileArgCount < 2)
	{
		PRINT_ERROR("No output file specified.");
		optsInvalid = true;
	}
	
	// If not in batch mode, should have no more than 2 file args
	if (!batchMode && fileArgCount > 2)
	{
		PRINT_ERROR("Too many arguments given. Did you mean to use batch mode (-b)?");
		optsInvalid = true;
	}
	
	// Output format needs to be specified
	if (formatOutput == DefaultFormat)
	{
		PRINT_ERROR("No output format specified.");
		optsInvalid = true;
	}
	
	// Print usage info if improperly used
	if (optsInvalid)
	{
		printUsage(argv[0]);
		return 1;
	}
	
	// Get encoder ready for all input files
	Encoder encoder(formatOutput, etc1quality);
	
	if (printInfo)
	{
		return 0;
	}
	
	if (!batchMode)
	{
		std::string inputFilePath = argv[optind];
		std::string outputFilePath = argv[optind + 1];
		try
		{
			std::cout << "Processing " << inputFilePath << " ..." << std::endl;
			
			Decoder decoder(inputFilePath, formatInput);
			encoder.processDecodedData(decoder);
			encoder.saveToFile(outputFilePath, useHeader);
			
			printf("[" CYAN "%s" RESET " -> " CYAN "%s" RESET "] " GREEN "%s\n" RESET,
			       stringFromFormat(decoder.getPixelFormat()).c_str(),
				   stringFromFormat(encoder.getEncodedFormat()).c_str(),
				   outputFilePath.c_str()
			);
		}
		catch(int e)
		{
			PRINT_ERROR("Failed to process file.");
			return 1;
		}
		return 0;
	}
	
	// Loop through input files (batch mode)
	for (int i = optind; i < argc; i++)
	{
		std::string filePath = argv[i];
		std::string fileName = filePath;
		
		// Get file directory
		std::string dirName;
		size_t lastSlash = filePath.rfind(PATH_SEPARATOR);
		if (lastSlash != std::string::npos)
		{
			dirName = filePath.substr(0, lastSlash + 1);
			fileName = filePath.substr(lastSlash + 1);
		}
	
		// Get file name without extension
		std::string fileNameNoExt;
		size_t lastDot = fileName.rfind(".");
		if (lastDot != std::string::npos)
			fileNameNoExt = fileName.substr(0, lastDot);
		
		// Determine output file name
		std::string ext = (formatOutput == PNG) ? ".png" : ".bin";
		std::string outputFileName = fileNameNoExt + ext;
		
		try
		{
			std::cout << "Processing " << filePath << " ..." << std::endl;
			std::string outputFilePath = (outputDir.empty() ? dirName : outputDir) + outputFileName;
			
			Decoder decoder(filePath, formatInput);
			encoder.processDecodedData(decoder);
			if (outputFileName == fileName)
				outputFileName = fileNameNoExt + "." + stringFromFormat(encoder.getEncodedFormat()) + ext;
			encoder.saveToFile(outputFilePath, useHeader);
			
			printf("[" CYAN "%s" RESET " -> " CYAN "%s" RESET "] " GREEN "%s\n" RESET,
			       stringFromFormat(decoder.getPixelFormat()).c_str(),
				   stringFromFormat(encoder.getEncodedFormat()).c_str(),
				   outputFilePath.c_str()
			);
		}
		catch(int e)
		{
			PRINT_ERROR("Failed to process file.");
		}
	}
	return 0;
}
