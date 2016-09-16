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
	fprintf(stderr,
		"\nUsage:\n"
		"  %s [options] files ...\n\n"
		"Options:\n"
		"  -r           Raw output without header. Header is added by default.\n"
		"  -p           Print info of input files instead of encoding them.\n"
		"  -h           Print this help information.\n"
		"  -d <dir>     Directory to output files. Defaults to directory of inputs.\n"
		"  -c <level>   Quality level for ETC1 compression:\n"
		"                 " CYAN "1" RESET "  - Low quality    (fast)\n"
		"                 " CYAN "2" RESET "  - Medium quality (default)\n"
		"                 " CYAN "3" RESET "  - High quality   (slow)\n"
		"  -i <format>  Input format type. Only needed when input files have no header.\n"
		"               Possible types: " CYAN "rgba8, rgb8, rgba5551, rgb565, rgba4, la8, hilo8,\n"
		"               l8, a8, la4, l4, a4, etc1, etc1a4" RESET ".\n"
		"  -o <format>  Output format type. Can be any of the above input types with some\n"
		"               additional special types:\n"
		"                 " CYAN "auto-etc1" RESET "  - ETC1 when input has no alpha, otherwise ETC1A4.\n"
		"                 " CYAN "auto-l8" RESET "    - L8 when input has no alpha, otherwise LA8.\n"
		"                 " CYAN "auto-l4" RESET "    - L4 when input has no alpha, otherwise LA4.\n"
		, progName);
}

int main(int argc, char **argv)
{
	int opt;
	bool optsInvalid = false;

	// Default options
	Options options;
	options.useHeader = true;
	options.threadCount = std::thread::hardware_concurrency() ? : 4;
	options.etc1quality = 2;
	options.formatInput = DefaultFormat;
	options.formatOutput = DefaultFormat;
	bool printInfo = false;

	while ((opt = getopt(argc, argv, "rphd:c:i:o:")) != -1)
	{
		 switch (opt)
		 {
			// Use raw output
			case 'r':
				options.useHeader = false;
				break;
			// Use raw output
			case 'p':
				printInfo = true;
				break;
			// Print usage help and exit
			case 'h':
				printUsage(argv[0]);
				return 0;
			// Use different directory for output
			case 'd':
				struct stat st;
				options.outputDir = optarg;
				if ((stat(options.outputDir.c_str(), &st) != 0) || !(st.st_mode & S_IFDIR))
				{
					PRINT_ERROR(options.outputDir << " - Not found or not valid directory.");
					return 1;
				}
				if (options.outputDir.back() != PATH_SEPARATOR)
					options.outputDir += PATH_SEPARATOR;
				break;
			// ETC1 compression level
			case 'c':
				try
				{
					options.etc1quality = std::stoi(optarg, nullptr, 10);
					if (options.etc1quality < 0 || options.etc1quality > 3)
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
				options.formatInput = formatFromString(optarg);
				if (options.formatInput == DefaultFormat ||
					options.formatInput == AutoETC1)
				{
					std::cerr << "Invalid input format: " << options.formatInput << std::endl;
					return 1;
				}
				break;
			// Define format for output file(s)
			case 'o':
				options.formatOutput = formatFromString(optarg);
				if (options.formatOutput == DefaultFormat)
				{
					std::cerr << "Invalid output format: " << options.formatOutput << std::endl;
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

	// Need at least 1 input arg
	if (argc - optind < 1)
	{
		PRINT_ERROR("No input files specified.");
		optsInvalid = true;
	}
	
	// Output format needs to be specified
	if (options.formatOutput == DefaultFormat)
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
	Encoder encoder(options.formatOutput, options.etc1quality);
	
	// Loop through input files
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
		std::string outputFileName = fileNameNoExt + ".bin";
		if (outputFileName == fileName)
			outputFileName = fileNameNoExt + ".out.bin";
		
		try
		{
			std::cout << "Processing " << filePath << " ..." << std::endl;
			std::string outputFilePath = (options.outputDir.empty() ? dirName : options.outputDir) + outputFileName;
			
			Decoder decoder(filePath, options.formatInput);
			encoder.processDecodedData(decoder);
			encoder.saveToFile(outputFilePath, options.useHeader);
			std::cout << "[" CYAN << stringFromFormat(encoder.getEncodedFormat()) << RESET "] " GREEN << outputFilePath << RESET << std::endl;
		}
		catch(int e)
		{
			PRINT_ERROR("Failed to process file.");
		}
	}
	return 0;
}
