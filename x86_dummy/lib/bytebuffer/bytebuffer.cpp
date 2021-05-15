/**
   ByteBuffer
   ByteBuffer.cpp
   Copyright 2011 - 2013 Ramsey Kant

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "bytebuffer.h"
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

/**
 * ByteBuffer constructor
 * Reserves specified size in internal vector
 *
 * @param size Size (in bytes) of space to preallocate internally. Default is set in DEFAULT_SIZE
 */
ByteBuffer::ByteBuffer(uint32_t size) {
	buf.reserve(size);
	clear();
#ifdef BB_UTILITY
	name = "";
#endif
}

/**
 * ByteBuffer constructor
 * Consume an entire uint8_t array of length len in the ByteBuffer
 *
 * @param arr uint8_t array of data (should be of length len)
 * @param size Size of space to allocate
 */
ByteBuffer::ByteBuffer(uint8_t* arr, uint32_t size) {
	// If the provided array is NULL, allocate a blank buffer of the provided size
	if (arr == NULL) {
		buf.reserve(size);
		clear();
	}
	else { // Consume the provided array
		buf.reserve(size);
		clear();
		putBytes(arr, size);
	}

#ifdef BB_UTILITY
	name = "";
#endif
}

/**
 * ByteBuffer Deconstructor
 *
 */
ByteBuffer::~ByteBuffer() {
}

/**
 * Bytes Remaining
 * Returns the number of bytes from the current read position till the end of the buffer
 *
 * @return Number of bytes from rpos to the end (size())
 */
uint32_t ByteBuffer::bytesRemaining() {
	return size() - rpos;
}

/**
 * Clear
 * Clears out all data from the internal vector (original preallocated size remains), resets the positions to 0
 */
void ByteBuffer::clear() {
	rpos = 0;
	wpos = 0;
	buf.clear();
}

/**
 * Clone
 * Allocate an exact copy of the ByteBuffer on the heap and return a pointer
 *
 * @return A pointer to the newly cloned ByteBuffer. NULL if no more memory available
 */
ByteBuffer* ByteBuffer::clone() {
	ByteBuffer* ret = new ByteBuffer(buf.size());

	// Copy data
	for (uint32_t i = 0; i < buf.size(); i++) {
		ret->put((uint8_t)get(i));
	}

	// Reset positions
	ret->setReadPos(0);
	ret->setWritePos(0);

	return ret;
}

/**
 * Equals, test for data equivilancy
 * Compare this ByteBuffer to another by looking at each byte in the internal buffers and making sure they are the same
 *
 * @param other A pointer to a ByteBuffer to compare to this one
 * @return True if the internal buffers match. False if otherwise
 */
bool ByteBuffer::equals(ByteBuffer* other) {
	// If sizes aren't equal, they can't be equal
	if (size() != other->size())
		return false;

	// Compare byte by byte
	uint32_t len = size();
	for (uint32_t i = 0; i < len; i++) {
		if ((uint8_t)get(i) != (uint8_t)other->get(i))
			return false;
	}

	return true;
}

/**
 * Resize
 * Reallocates memory for the internal buffer of size newSize. Read and write positions will also be reset
 *
 * @param newSize The amount of memory to allocate
 */
void ByteBuffer::resize(uint32_t newSize) {
	buf.resize(newSize);
	rpos = 0;
	wpos = 0;
}

/**
 * Size
 * Returns the size of the internal buffer...not necessarily the length of bytes used as data!
 *
 * @return size of the internal buffer
 */
uint32_t ByteBuffer::size() {
	return buf.size();
}

// Replacement

/**
 * Replace
 * Replace occurance of a particular uint8_t, key, with the uint8_t rep
 *
 * @param key uint8_t to find for replacement
 * @param rep uint8_t to replace the found key with
 * @param start Index to start from. By default, start is 0
 * @param firstOccuranceOnly If true, only replace the first occurance of the key. If false, replace all occurances. False by default
 */
void ByteBuffer::replace(uint8_t key, uint8_t rep, uint32_t start, bool firstOccuranceOnly) {
	uint32_t len = buf.size();
	for (uint32_t i = start; i < len; i++) {
		uint8_t data = read<uint8_t>(i);
		// Wasn't actually found, bounds of buffer were exceeded
		if ((key != 0) && (data == 0))
			break;

		// Key was found in array, perform replacement
		if (data == key) {
			buf[i] = rep;
			if (firstOccuranceOnly)
				return;
		}
	}
}

// Read Functions

uint8_t ByteBuffer::peek() {
	return read<uint8_t>(rpos);
}

uint8_t ByteBuffer::get() {
	return read<uint8_t>();
}

uint8_t ByteBuffer::get(uint32_t index) {
	return read<uint8_t>(index);
}

void ByteBuffer::getBytes(uint8_t* buf, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		buf[i] = read<uint8_t>();
	}
}

void ByteBuffer::getBytes(uint8_t* buf, uint32_t index, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		buf[i] = read<uint8_t>(index + i);
	}
}

std::wstring ByteBuffer::getWString()
{
	auto* buffer = (wchar_t*)(buf.data() + rpos);

	auto strLength = wcslen(buffer);

	rpos += strLength * 2 + 2;

	return std::wstring(buffer, strLength);
}

std::wstring ByteBuffer::getWString(int size)
{
	auto* buffer = (wchar_t*)(buf.data() + rpos);

	auto strLength = wcslen(buffer);

	rpos += (strLength * 2 + 2) + (size - (strLength * 2 + 2));

	return std::wstring(buffer, strLength);
}

std::string ByteBuffer::getString()
{
	auto* buffer = (char*)(buf.data() + rpos);

	auto strLength = strlen(buffer);

	rpos += strLength;

	return std::string(buffer, strLength);
}

std::string ByteBuffer::getString(int size)
{
	auto* buffer = (char*)(buf.data() + rpos);

	auto strLength = size;

	rpos += strLength;

	return std::string(buffer, strLength);
}

char ByteBuffer::getChar() {
	return read<char>();
}

char ByteBuffer::getChar(uint32_t index) {
	return read<char>(index);
}

double ByteBuffer::getDouble() {
	return read<double>();
}

double ByteBuffer::getDouble(uint32_t index) {
	return read<double>(index);
}

float ByteBuffer::getFloat() {
	return read<float>();
}

float ByteBuffer::getFloat(uint32_t index) {
	return read<float>(index);
}

int ByteBuffer::getInt() {
	return read<int>();
}

int ByteBuffer::getInt(uint32_t index) {
	return read<int>(index);
}

unsigned int ByteBuffer::getUInt() {
	return read<unsigned int>();
}

unsigned int ByteBuffer::getUInt(uint32_t index) {
	return read<unsigned int>(index);
}

uint64_t ByteBuffer::getLong() {
	return read<uint64_t>();
}

uint64_t ByteBuffer::getLong(uint32_t index) {
	return read<uint64_t>(index);
}

uint16_t ByteBuffer::getShort() {
	return read<uint16_t>();
}

uint16_t ByteBuffer::getShort(uint32_t index) {
	return read<uint16_t>(index);
}


// Write Functions

void ByteBuffer::put(ByteBuffer* src) {
	uint32_t len = src->size();
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(src->get(i));
}

void ByteBuffer::put(uint8_t b) {
	append<uint8_t>(b);
}

void ByteBuffer::put(uint8_t b, uint32_t index) {
	insert<uint8_t>(b, index);
}

void ByteBuffer::putWString(std::wstring b, uint32_t size)
{
	int written = 0;

	for (int i = 0; i < b.length(); i++)
	{
		append<uint8_t>(b[i]);
		append<uint8_t>(0x00);

		written += 2;
	}

	putEmptyBytes(size - written);
}

void ByteBuffer::putString(std::string b, uint32_t size)
{
	int written = 0;

	for (int i = 0; i < b.length(); i++)
	{
		append<uint8_t>(b[i]);

		written++;
	}

	putEmptyBytes(size - written);
}

void ByteBuffer::putBytesFromFile(std::string file, int offset) {
	//open file
	std::ifstream infile(file);

	//get length of file
	infile.seekg(0, infile.end);
	size_t length = infile.tellg();
	infile.seekg(0, infile.beg);

	char* buffer = new char[length];

	//read file
	infile.read(buffer, length);

	for (uint32_t i = offset; i < length; i++)
		append<uint8_t>(buffer[i]);
}

void ByteBuffer::putBytes(uint8_t* b, uint32_t len) {
	// Insert the data one byte at a time into the internal buffer at position i+starting index
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(b[i]);
}

void ByteBuffer::putBytes(uint8_t* b, uint32_t len, uint32_t index) {
	wpos = index;

	// Insert the data one byte at a time into the internal buffer at position i+starting index
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(b[i]);
}

void ByteBuffer::putEmptyBytes(uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(0x0);
}

void ByteBuffer::putChar(char value) {
	append<char>(value);
}

void ByteBuffer::putChar(char value, uint32_t index) {
	insert<char>(value, index);
}

void ByteBuffer::putDouble(double value) {
	append<double>(value);
}

void ByteBuffer::putDouble(double value, uint32_t index) {
	insert<double>(value, index);
}
void ByteBuffer::putFloat(float value) {
	append<float>(value);
}

void ByteBuffer::putFloat(float value, uint32_t index) {
	insert<float>(value, index);
}

void ByteBuffer::putInt(int value) {
	append<int>(value);
}

void ByteBuffer::putInt(int value, uint32_t index) {
	insert<int>(value, index);
}

void ByteBuffer::putUInt(unsigned int value) {
	append<unsigned int>(value);
}

void ByteBuffer::putUInt(unsigned int value, uint32_t index) {
	insert<unsigned int>(value, index);
}

void ByteBuffer::putLong(uint64_t value) {
	append<uint64_t>(value);
}


void ByteBuffer::putLong(uint64_t value, uint32_t index) {
	insert<uint64_t>(value, index);
}

void ByteBuffer::putShort(uint16_t value) {
	append<uint16_t>(value);
}

void ByteBuffer::putShort(uint16_t value, uint32_t index) {
	insert<uint16_t>(value, index);
}

// Utility Functions
#ifdef BB_UTILITY
void ByteBuffer::setName(std::string n) {
	name = n;
}

std::string ByteBuffer::getName() {
	return name;
}

void ByteBuffer::printInfo() {
}

void ByteBuffer::printAH()
{

}

void ByteBuffer::printAscii() {

}

std::string ByteBuffer::printHex()
{
	std::stringstream res;
	uint32_t length = buf.size();
	for (uint32_t i = 0; i < length; i++)
	{
		res << std::hex << (int)buf[i] << " ";
	}
	return res.str();
}

void ByteBuffer::printPosition() {
}
#endif