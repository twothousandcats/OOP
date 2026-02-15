#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <array>

constexpr size_t MATRIX_DIMENSION = 3;

using Matrix3x3 = std::array<std::array<double, MATRIX_DIMENSION>, MATRIX_DIMENSION>;

struct Args
{
	std::string inputFile;
	bool showHelp = false;
};

void PrintHelp()
{
	std::cout << "Usage:\n";
	std::cout << "	WinOS:\n";
	std::cout << "	  invert.exe               # Read matrix from stdin\n";
	std::cout << "	  invert.exe <filename>    # Read matrix from file\n";
	std::cout << "	  invert.exe -h            # Show this help\n";
	std::cout << "	MacOS/Linux:\n";
	std::cout << "	  ./invert               # Read matrix from stdin\n";
	std::cout << "	  ./invert <filename>    # Read matrix from file\n";
	std::cout << "	  ./invert -h            # Show this help\n";
}

bool ParseArgs(const int argc, char* argv[], Args& args)
{
	if (argc == 2 && std::string(argv[1]) == "-h")
	{
		args.showHelp = true;
		return true;
	}

	if (argc == 1)
	{
		args.inputFile.clear();
		return true;
	}

	if (argc == 2)
	{
		args.inputFile = argv[1];
		return true;
	}

	return false;
}

bool ReadMatrix(std::istream& input, Matrix3x3& matrix)
{
	for (size_t i = 0; i < MATRIX_DIMENSION; ++i)
	{
		std::string line;
		if (!std::getline(input, line))
		{
			return false;
		}

		std::istringstream strStream(line);
		for (size_t j = 0; j < MATRIX_DIMENSION; ++j)
		{
			if (!(strStream >> matrix[i][j]))
			{
				return false;
			}
		}

		// no extra values on the line
		if (std::string remaining; strStream >> remaining)
		{
			return false;
		}
	}

	// no extra lines
	if (std::string extraLine; std::getline(input, extraLine))
	{
		std::istringstream extraStrStream(extraLine);
		if (std::string extra; extraStrStream >> extra)
		{
			return false;
		}
	}

	return true;
}

double CalculateDeterminant(const Matrix3x3& matrix)
{
	return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
		- matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
		+ matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

double GetMinor(const Matrix3x3& matrix, const size_t excludeRow, const size_t excludeCol)
{
	size_t rows[MATRIX_DIMENSION - 1], cols[MATRIX_DIMENSION - 1];
	size_t rowIndex = 0, colIndex = 0;
	for (size_t i = 0; i < MATRIX_DIMENSION; i++)
	{
		if (i != excludeRow)
		{
			rows[rowIndex++] = i;
		}
		if (i != excludeCol)
		{
			cols[colIndex++] = i;
		}
	}

	return matrix[rows[0]][cols[0]] * matrix[rows[1]][cols[1]] -
		matrix[rows[0]][cols[1]] * matrix[rows[1]][cols[0]];
}

// Algebraic complement
double GetCofactor(const Matrix3x3& matrix, const size_t row, const size_t col)
{
	const double minor = GetMinor(matrix, row, col);
	// Знак зависит от четности суммы индексов
	// The sign depends on the parity of the sum of the indices: (-1)^(i+j)
	return (row + col) % 2 == 0
		? minor
		: -minor;
}

Matrix3x3 CalculateInverse(const Matrix3x3& matrix)
{
	Matrix3x3 cofactorMatrix;
	// Вычисляем матрицу алгебраических дополнений
	// Calculate algebraic complement matrix
	for (size_t i = 0; i < MATRIX_DIMENSION; ++i)
	{
		for (size_t j = 0; j < MATRIX_DIMENSION; ++j)
		{
			cofactorMatrix[i][j] = GetCofactor(matrix, i, j);
		}
	}

	// Транспонируем матрицу алгебраических дополнений (присоединенная матрица)
	// и делим на определитель
	Matrix3x3 inverse;
	const double determinant = CalculateDeterminant(matrix);
	for (size_t i = 0; i < MATRIX_DIMENSION; ++i)
	{
		for (size_t j = 0; j < MATRIX_DIMENSION; ++j)
		{
			inverse[i][j] = cofactorMatrix[j][i] / determinant; // [j][i] для транспонирования
		}
	}

	return inverse;
}

void PrintInvertedMatrix(const Matrix3x3& matrix)
{
	std::cout << std::fixed << std::setprecision(3);
	for (size_t i = 0; i < MATRIX_DIMENSION; ++i)
	{
		for (size_t j = 0; j < MATRIX_DIMENSION; ++j)
		{
			if (j > 0)
			{
				std::cout << "\t";
			}
			std::cout << matrix[i][j];
		}
		std::cout << "\n";
	}
}

int main(int argc, char* argv[])
{
	Args args;
	if (!ParseArgs(argc, argv, args))
	{
		std::cout << "Invalid arguments. Use -h for help.\n";
		return 1;
	}

	if (args.showHelp)
	{
		PrintHelp();
		return 0;
	}

	std::ifstream fileStream;
	std::istream* input = &std::cin; // unify data reading, by istream pointer
	if (!args.inputFile.empty())
	{
		fileStream.open(args.inputFile);
		if (!fileStream.is_open())
		{
			std::cout << "Invalid matrix\n";
			return 1;
		}
		input = &fileStream;
	}

	Matrix3x3 matrix;
	if (!ReadMatrix(*input, matrix))
	{
		std::cout << "Invalid matrix format\n";
		return 1;
	}

	if (const double determinant = CalculateDeterminant(matrix); determinant == 0.0)
	{
		std::cout << "Non-invertible\n";
		return 0;
	}

	PrintInvertedMatrix(CalculateInverse(matrix));

	return 0;
}