#pragma once

#include <string>
#include <vector>

class IBarcodeDecoder
{
	virtual bool Init() = 0;
	virtual bool DecodeImage(PTCHAR ImageFileName, std::vector<void *>DecodedBuffers) = 0;
};