#ifndef COCOA_EDITOR_SCRIPT_PARSER_H
#define COCOA_EDITOR_SCRIPT_PARSER_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include <filesystem>

namespace Cocoa
{
	struct UVariable
	{
		std::string type;
		std::string identifier;
		// void* m_Literal;
	};

	struct UClass
	{
		std::string className;
		std::filesystem::path fullFilepath;
		std::list<UVariable> variables;
	};

	struct UStruct
	{
		std::string structName;
		const std::filesystem::path& fullFilepath;
		std::list<UVariable> variables;
	};

	class ScriptParser
	{
	public:
		std::string generateHeaderFile();
		void debugPrint();
		void parse();

		bool canGenerateHeaderFile() const { return mStructs.size() != 0 || mClasses.size() != 0; }
		std::vector<UClass>& getClasses() { return mClasses; }

		static std::string getFilenameAsClassName(std::string filename);

	private:
		int mCurrentToken;
		std::filesystem::path mFullFilepath;

		std::vector<UClass> mClasses;
		std::vector<UStruct> mStructs;
	};
}

#endif
