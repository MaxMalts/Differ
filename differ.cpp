#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstring>
#include <assert.h>


/*
	Holds binary data.
*/
class Buffer {
public:

	Buffer() = default;


	Buffer(size_t size) {
		buffer = new char[size];
	}


	Buffer(const std::string& fName) {
		assert(fName.size() > 0);

		ReadFile(fName);
	}


	void ReadFile(const std::string& fName) {
		assert(fName.size() > 0);

		if (buffer != nullptr) {
			delete[] buffer;
		}
		
		std::ifstream file(fName, std::ios::binary);

		file.seekg(0, std::ios::end);
		size = file.tellg();
		file.seekg(0, std::ios::beg);

		buffer = new char[size];
		file.read(buffer, size);

		file.close();
	}


	char operator[](size_t pos) const {
		assert(buffer != nullptr);
		assert(pos < size);

		return buffer[pos];
	}


	const char* CString() const {
		return buffer;
	}


	char* CString() {
		return buffer;
	}


	int Size() const {
		return size;
	}


	~Buffer() {
		if (buffer != nullptr) {
			delete[] buffer;
		}
	}


private:

	char* buffer = nullptr;
	size_t size = 0;
};


/*
	Holds the information about one common element in the longest comon subsequence
*/
struct CommonElement {
	int oldPos = 0;
	int newPos = 0;
};

/*
	Calculates the longest common subsequence of two sequences.
	Returns common elements in ascending order of positions
*/
void LCS(const Buffer& oldF, const Buffer& newF, std::vector<CommonElement>& commonElems) {
	assert(oldF.Size() > 0);
	assert(newF.Size() > 0);

	commonElems.clear();

	int oldSize = oldF.Size();
	int newSize = newF.Size();
	std::vector<std::vector<int>> lcsTable(oldSize, std::vector<int>(newSize));

	for (int i = 0; i <= oldSize; i++) {
		for (int j = 0; j <= newSize; j++) {
			if (i == 0 || j == 0) {
				lcsTable[i][j] = 0;
			} else if (oldF[i - 1] == newF[j - 1]) {
				lcsTable[i][j] = lcsTable[i - 1][j - 1] + 1;
			} else {
				lcsTable[i][j] = std::max(lcsTable[i - 1][j], lcsTable[i][j - 1]);
			}
		}
	}

	int resSize = lcsTable[oldSize][newSize];
	commonElems.resize(resSize);

	int ind = resSize;

	int i = oldSize, j = newSize;
	while (i > 0 && j > 0) {
		if (oldF[i - 1] == newF[j - 1]) {
			commonElems[ind - 1].oldPos = i - 1;
			commonElems[ind - 1].newPos = j - 1;
			--i;
			--j;
			--ind;
		} else if (lcsTable[i - 1][j] > lcsTable[i][j - 1]) {
			--i;
		} else {
			--j;
		}
	}
}


/*
	Compares two files and creates a diff file.
*/
void CreateDiff(const std::string& oldFName, const std::string& newFName, const std::string& diffFName) {
	std::ofstream diffF(diffFName, std::ios::binary);

	Buffer oldBuf(oldFName);
	Buffer newBuf(newFName);

	std::vector<CommonElement> commonElems;
	LCS(oldBuf, newBuf, commonElems);

	for (int i = 1; i < commonElems.size(); ++i) {
		int oldOffset = commonElems[i].oldPos - commonElems[i - 1].oldPos;
		int newOffset = commonElems[i].newPos - commonElems[i - 1].newPos;

		if (oldOffset > 1 || newOffset > 1) {
			diffF << commonElems[i - 1].oldPos + 1 << oldOffset - 1 << newOffset - 1;
			diffF.write(newBuf.CString() + commonElems[i - 1].newPos, newOffset - 1);
		}
	}

	diffF.close();
}


/*
	Creates the new file via old and diff files.
*/
void Restore(const std::string& oldFName, const std::string& diffFName, const std::string& newFName) {
	Buffer oldBuf(oldFName);
	std::ofstream newF(newFName, std::ios::binary);
	std::ifstream diffF(diffFName, std::ios::binary);

	int oldCursor = 0;

	int oldPos = 0, oldSize = 0, newSize = 0;
	diffF >> oldPos >> oldSize >> newSize;

	while (!diffF.eof()) {
		assert(oldPos >= 0);
		assert(oldSize > 0);
		assert(newSize > 0);

		int unmodifiedSize = oldPos - oldCursor;
		newF.write(oldBuf.CString() + oldCursor, unmodifiedSize);
		oldCursor += unmodifiedSize;

		Buffer buf(newSize);
		diffF.read(buf.CString(), newSize);
		newF.write(buf.CString(), newSize);
		oldCursor += oldSize;

		diffF >> oldPos >> oldSize >> newSize;
	}

	newF.close();
	diffF.close();
}


int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cout << "Wrong number of arguments.\n";
		return 1;
	}

	if (std::strcmp(argv[1], "calculate diff") == 0) {
		CreateDiff(argv[2], argv[3], argv[4]);
	} else if (std::strcmp(argv[1], "Restore") == 0) {
		Restore(argv[2], argv[3], argv[4]);
	} else {
		std::cout << "wrong command.\n";
		return 1;
	}

	return 0;
}