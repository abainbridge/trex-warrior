#ifndef INCLUDE_TEXT_FILE_READER_H
#define INCLUDE_TEXT_FILE_READER_H

#include <stdio.h>


// This class provides an RAII wrapper to a FILE and provides a simple tokenizer
// that can read lines of any length.
class TextFileReader
{
private:
	FILE *m_file;

	char m_seperatorChars[16];
	char m_filename[256];

	int	m_offsetIndex;

	void DoubleMaxLineLen();
	void CleanLine();			// Strip comments

public:
	int	m_tokenIndex;
	char *m_line;
	unsigned int m_maxLineLen;		// Doesn't include '\0' - m_line points to an array one byte larger than this
	unsigned int m_lineNum;

	TextFileReader(char const *_filename);
	~TextFileReader();

    bool IsOpen();
	bool ReadLine();	// Returns false on EOF, true otherwise
	bool TokenAvailable();
	char *GetNextToken();
    char *GetRestOfLine();

	char const *GetFilename();

	void SetSeperatorChars(char const *_seperatorChars);
	void SetDefaultSeperatorChars();
};


#endif
