#pragma once

class StringConverter
{
public:
	static std::wstring StringToWide(std::string str);
	static std::string GetLastErrorAsString();
};