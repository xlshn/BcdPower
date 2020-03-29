#pragma once

#include <Windows.h>

struct Part_Info
{
	PARTITION_STYLE PartStyle;
	LARGE_INTEGER   StartingOffset;
	wchar_t PartLetter;
};

class CPart
{
public:
	CPart();
private:
	Part_Info PartInfo;
};