#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

constexpr int MATRIX_CAPACITY = 3;

constexpr auto* USAGE_INFO_EXE_SAMPLE_WIN = "   WINDOWS OS: invert.exe";
constexpr auto* USAGE_INFO_EXE_SAMPLE_UNIX = "   MacOS/Linux: ./invert:";
constexpr auto* USAGE_INFO_HEADING = "Использование:";
constexpr auto* USAGE_INFO_HELP = " -h";
constexpr auto* USAGE_INFO_STDIN = " далее программа ожидает коэффициенты матрицы";
constexpr auto* USAGE_INFO_FILE = " <file_name.txt>";

constexpr auto* OPENING_ERROR = "Ошибка открытия файла: ";
constexpr auto* INVALID_M_FORMAT = "Invalid matrix format";
constexpr auto* NON_INVERTIBLE = "Non-invertible";

bool ReadMatrix(std::istream& in, double mat[MATRIX_CAPACITY][MATRIX_CAPACITY])
{
	std::string line;
	for (int i = 0; i < MATRIX_CAPACITY; ++i)
	{
		if (!getline(in, line))
			return false;
		std::stringstream ss(line);
		for (int j = 0; j < MATRIX_CAPACITY; ++j)
		{
			std::string token;
			if (!getline(ss, token, '\t'))
				return false;
			try
			{
				mat[i][j] = std::stod(token);
			}
			catch (...)
			{
				return false;
			}
		}
	}
	return true;
}

double GetDeterminant(double mat[MATRIX_CAPACITY][MATRIX_CAPACITY])
{
	return mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1])
		- mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
		+ mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
}

void Cofactor(double mat[MATRIX_CAPACITY][MATRIX_CAPACITY], double inv[MATRIX_CAPACITY][MATRIX_CAPACITY])
{
	for (int i = 0; i < MATRIX_CAPACITY; ++i)
	{
		for (int j = 0; j < MATRIX_CAPACITY; ++j)
		{
			const int sign = ((i + j) % 2 == 0) ? 1 : -1;
			const int i1 = (i + 1) % MATRIX_CAPACITY;
			const int i2 = (i + 2) % MATRIX_CAPACITY;
			const int j1 = (j + 1) % MATRIX_CAPACITY;
			const int j2 = (j + 2) % MATRIX_CAPACITY;

			double detMinor = mat[i1][j1] * mat[i2][j2] - mat[i1][j2] * mat[i2][j1];
			inv[j][i] = sign * detMinor; // Transpose
		}
	}
}

bool Inverse(double mat[MATRIX_CAPACITY][MATRIX_CAPACITY], double inv[MATRIX_CAPACITY][MATRIX_CAPACITY])
{
	const double determinant = GetDeterminant(mat);
	if (abs(determinant) < 1e-9)
	{
		return false;
	}

	Cofactor(mat, inv);
	for (int i = 0; i < MATRIX_CAPACITY; ++i)
	{
		for (int j = 0; j < MATRIX_CAPACITY; ++j)
		{
			inv[i][j] /= determinant;
		}
	}

	return true;
}

void PrintMatrix(double mat[MATRIX_CAPACITY][MATRIX_CAPACITY])
{
	std::cout << std::fixed << std::setprecision(3);
	for (int i = 0; i < MATRIX_CAPACITY; ++i)
	{
		for (int j = 0; j < MATRIX_CAPACITY; ++j)
		{
			std::cout << mat[i][j];
			if (j != MATRIX_CAPACITY - 1)
				std::cout << "\t";
		}

		std::cout << std::endl;
	}
}

int main(const int argCount, char* argValue[])
{
	if (argCount > 1)
	{
		const std::string argument = argValue[1];

		if (argument == "-h")
		{
			std::cout << USAGE_INFO_HEADING << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_WIN << USAGE_INFO_HELP << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_UNIX << USAGE_INFO_HELP << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_WIN << USAGE_INFO_STDIN << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_UNIX << USAGE_INFO_STDIN << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_WIN << USAGE_INFO_FILE << std::endl;
			std::cout << USAGE_INFO_EXE_SAMPLE_UNIX << USAGE_INFO_FILE << std::endl;

			return 0;
		}

		std::ifstream file(argument);
		if (!file.is_open())
		{
			std::cerr << OPENING_ERROR << argument << std::endl;
			return 1;
		}

		double mat[MATRIX_CAPACITY][MATRIX_CAPACITY];
		if (!ReadMatrix(file, mat))
		{
			std::cout << INVALID_M_FORMAT << std::endl;
			return 0;
		}
		file.close();

		double inv[MATRIX_CAPACITY][MATRIX_CAPACITY];
		if (!Inverse(mat, inv))
		{
			std::cout << NON_INVERTIBLE << std::endl;
			return 0;
		}

		PrintMatrix(inv);
	}
	else
	{
		double mat[MATRIX_CAPACITY][MATRIX_CAPACITY];
		if (!ReadMatrix(std::cin, mat))
		{
			std::cout << INVALID_M_FORMAT << std::endl;
			return 0;
		}

		double inv[MATRIX_CAPACITY][MATRIX_CAPACITY];
		if (!Inverse(mat, inv))
		{
			std::cout << NON_INVERTIBLE << std::endl;
			return 0;
		}

		PrintMatrix(inv);
	}

	return 0;
}