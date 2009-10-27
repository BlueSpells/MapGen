#pragma once

#include <string>
#include <vector>


struct SBarcodeData
{
	BYTE *Data;
	int  DataLength;
};


class IBarcodeDecoder
{
	virtual bool Init() = 0;
	virtual bool DecodeImage(PTCHAR ImageFileName, std::vector<SBarcodeData> &DecodedBuffers) = 0;
};