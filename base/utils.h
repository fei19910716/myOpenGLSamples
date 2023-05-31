/*

        Copyright 2014 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


#ifndef _WIN64
#include <unistd.h>
#endif
#include <stdio.h>
#include <string>

typedef unsigned int uint;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))


bool ReadFile(const char* fileName, std::string& outFile);



void FileError(const char* pFileName, uint line, const char* pErrorMsg);
#define FILE_ERROR(ErrorMsg) FileError(__FILE__, __LINE__, ErrorMsg);