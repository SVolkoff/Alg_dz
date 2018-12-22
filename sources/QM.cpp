#include"QM.hpp"

int main(int argc, char *argv[])
{
	std::string str1; 
	std::ifstream input(argv[1]);
	std::ofstream output(argv[2]);
	if (!output.is_open())
		return 0;
	if (!input.is_open())
	{
		output << "Не найден входной файл";
		return 0;
	}
	std::getline(input, str1);
	size_t len = str1.size();
	if (len > 1073741824)
		output << "Слишком большое число переменных"; 
	bool correctlen = true; 
	if (!len)
	{ 
		output << "Входной файл пуст"; 
		return 0; 
	} 
	while (len > 1) 
	{
		if ((len % 2) == 1) correctlen = false;
		len >>= 1;
		variables++;
	} 
	if (correctlen)
	{
		BooleanFunction func;
		 func.MinimizeDNF(str1);
		 if (str1.size())
			 output << str1;
		 else
			 output << "Неправильно введен вектор значений";
	}
	else output << "Неправильный размер вектора значений"; 
	input.close();
	output.close();
	return 0;
}
