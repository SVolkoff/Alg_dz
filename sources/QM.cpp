#include"QM.hpp"

int main(int argc, char *argv[]) 
{
	std::string str1; 
	std::ifstream input("argv[1]");
	std::ofstream output("argv[2]"); 
	if (!input.is_open())
		output << "Не найден входной файл"; 
	if (!input.is_open())
		return 0;
	std::getline(input, str1);
	int len = str1.size();
	if (len > 1073741824)
		output << "Слишком большое число переменных"; 
	bool correctimput = true; 
	if (!len)
	{ 
		output << "Входной файл пуст"; 
		return 0; 
	} 
	while (len > 1) 
	{
		if ((len % 2) == 1) correctimput = false; 
		len >>= 1;
		variables++;
	} 
	if (correctimput) 
	{
		BooleanFunction func;
		output << func.QuineMcClaskey(str1); 
		return 0;
	}
	else output << "Неправильный размер вектора значений"; input.close(); output.close(); return 0;
}
