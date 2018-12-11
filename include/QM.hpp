#include <iostream> 
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

int variables = 0;	//кол-во переменных

class BooleanFunction 
{
private:

	struct Implicant
	{
		int glpos;				//позиция склеивания
		std::string bits;			// Битовая запись импликанта 
		int ones;		//Количество единиц 
		bool used;		//Использовалась ли импликанта в склейке 
		int nimplicant;    //Номер набора  
		std::string nameofterm;		//Название терма 
		std::vector<int> numofterms;		//Номера термов 
		bool operator<(const Implicant& im) const //переопределение оператора сравнения для импликант 
		{
			return ones < im.ones; //сравниваются кол-ва единиц 
		}
		Implicant(int imp = 0, std::vector<int> min = std::vector<int>(), std::string str = "", int gl = 0, bool use = false)
			: nimplicant(imp), glpos(gl), ones(0), used(use)
		{
			if (str == "")  // если строка термов пустая (инициализация)
				nameofterm = 'n' + std::to_string(imp); // задаем новое значение из номера терма 
			else nameofterm = str;   //иначе присваиваем строку  s
			if (min.empty()) numofterms.push_back(imp);  //если список термов пуст, инициализируем номером терма 
			else numofterms = min; //иначе присваиваем существующий 
			int bit = 1;		
			bit <<= variables;  //сдвигаем счетчик на кол-во переменных 
			while (bit >>= 1)	// цикл на lg(n)+1
				if (gl & bit)	//Если в позиции есть склейка, записываем *
					bits += '*';
				else if (imp & bit)
				{
					bits += '1'; ++ones; //терм записывается в bits 
				}
				else bits += '0';
		}

	};

	std::vector<int> imnumbers; //Вектор номеров импликант
	std::vector<Implicant> implicants; //Вектор импликант 
	bool notone = false, notnul = false;
public: std::string QuineMcClaskey(std::string str); int count1(size_t x);
};

int BooleanFunction::count1(size_t x)	//Считает количество единиц 
{
	int numof1 = 0; while (x) { numof1 += x % 2; x >>= 1; } return numof1;
}

std::string BooleanFunction::QuineMcClaskey(std::string str) {
	std::string answer; int combs = 1 << variables;	// побитово сдвигаем на количество переменных 
	for	(size_t i=0; i<str.size(); ++i) //O(2^n) 
	{
		if (str[i] == '1') 
		{ 
			implicants.push_back(i); // закидываем номера термов 
			imnumbers.push_back(i); notnul = true; //проверка на равенство 0 
		}
		else if (str[i] == '0')
			notone = true; //проверка на равенство 1 
		else return "Неправильно введен вектор значений";
	}
	if (!notnul)
		return "F=0";	//если вектор функции нулевой , вернем F=0 
	if (!notone)
		return "F=1"; //если вектор функции единичный, вернем F=1

	sort(implicants.begin(), implicants.end()); // сортируем импликанты по количеству единиц  

	std::vector<Implicant> temp; // Создаем два новых вектора импликант 
	std::vector<Implicant> finals;
	while (implicants.size() > 1) //Пока все возможные импликанты не будут склеены  
	{
		for (size_t i = 0; i < implicants.size() - 1; ++i) // Если импликанты повторяются, удаляем 
			for (size_t j = implicants.size() - 1; j > i; --j)
				if (implicants[j].bits == implicants[i].bits)
					implicants.erase(implicants.begin() + j);
		temp.clear();
		for (size_t i = 0; i < implicants.size() - 1; ++i) //O(n^2)
			for (size_t j = i + 1; j < implicants.size(); ++j)
				if (implicants[j].ones == implicants[i].ones + 1 && // Если мощности импликантов отличаются на 1
					implicants[j].glpos == implicants[i].glpos &&			// и у них стоят * в одних и тех же местах
					count1(implicants[i].nimplicant ^implicants[j].nimplicant) == 1)  // и расстояние хэмминга =1
				{
					implicants[i].used = true; // помечаем импликанты склеенными 
					implicants[j].used = true;
					std::vector<int> tempnum = implicants[i].numofterms;
					tempnum.insert(tempnum.end(), implicants[j].numofterms.begin(), implicants[j].numofterms.end());//собираем номера термов
					temp.push_back(
						Implicant(implicants[i].nimplicant, tempnum, // создаем новый импликант, склеивая старые, 
						implicants[i].nameofterm + implicants[j].nameofterm,		//закидываем номер импликанта, список термов, присваиваем название склеенного терма
						(implicants[i].nimplicant ^ implicants[j].nimplicant) | implicants[i].glpos));  // и позиции, на которых произошла склейка 
				}
		for (size_t i = 0; i < implicants.size(); ++i)
			if (!implicants[i].used) finals.push_back(implicants[i]); //Все импликанты, которые нельзя склеить, запишем в первичные
		implicants = temp;		//Продолжим работу со склеенными импликантами 
		sort(implicants.begin(), implicants.end());
	}
	for (size_t i = 0; i < implicants.size(); ++i)
		finals.push_back(implicants[i]); //закидываем склеенные импликанты 

	bool **table = new bool *[finals.size()]; //создаем таблицу покрытия термов 
	for (size_t i = 0; i < finals.size(); i++) //размера кол-ва финальных термов 
		table[i] = new bool[imnumbers.size()];  // на кол-во единиц в функции 
	for (size_t i = 0; i < finals.size(); ++i)  //заполнить таблицу нулями 
		for (size_t j = 0; j < imnumbers.size(); ++j)
			table[i][j] = false;
	for (size_t i = 0; i < finals.size(); ++i)
		for (size_t j = 0; j < finals[i].numofterms.size(); ++j)
			for (size_t k = 0; k < imnumbers.size(); ++k) // Заполняем таблицу 
				if (finals[i].numofterms[j] == imnumbers[k]) //Если первичный импликант покрывается, ставим 1
					table[i][k] = true;
	std::vector<size_t> P0, P1; //Создаем два вектора 
	for (size_t i = 0; i < finals.size(); ++i)
		if (table[i][0]) P0.push_back(1 << i); //если импликант покрывается, записываем 1 на позицию номера покрываемой строки
	std::vector<size_t> res;
	for (size_t k = 1; k < imnumbers.size(); ++k)  //Для каждого первичного импликанта 
	{
		P1.clear();
		for (size_t i = 0; i < finals.size(); ++i)
			if (table[i][k])
				P1.push_back(1 << i); //если значение не пусто, записываем 1, на позицию номера покрываемой строки
		
		for (size_t i = 0; i < P0.size(); ++i)
			for (size_t j = 0; j < P1.size(); ++j)
				res.push_back(P0[i] | P1[j]); //записывем сложение двух покрытий   
		sort(res.begin(), res.end());
		res.erase(unique(res.begin(), res.end()), res.end()); //удаляем повторения 
		for (size_t i = 0; i < res.size() - 1; ++i)
			for (size_t j = res.size() - 1; j > i; --j)
			{
				size_t mult = res[i] & res[j];
				if ((mult & res[i]) == res[i]) //поглощение 
					res.erase(res.begin() + j);
				else if ((mult & res[j]) == res[j])
				{
					size_t t = res[i];
					res[i] = res[j];
					res[j] = t;
					res.erase(res.begin() + j);
					j = res.size();
				}
			}
		P0 = res;
		res.clear();
	}
	int min = count1(P0[0]); //Количество импликантов, входящих в покрытие 
	size_t index = 0;
	for (size_t i = 1; i < P0.size(); ++i)
		if (min > count1(P0[i]))
		{
			min = count1(P0[i]);
			index = i;
		}
	bool sum = false;
	answer = "F = "; // Выводится минимальная ДНФ
	for (size_t i = 0; i < finals.size(); ++i)
	{
		if (P0[index] & (1 << i))
		{
			if (sum)
				answer += " + ";
			sum = true;
			int neg = 1;
			neg <<= variables;
			int	lit = 1;
			while (neg >>= 1)
			{
				if (!(finals[i].glpos & neg))
				{
					if (finals[i].nimplicant & neg)
						answer += "";
					else answer += "!";
					answer += "x" + std::to_string(lit); // Выписываем переменные 
				}
				++lit;
			}
		}
	}

	return answer;
}
