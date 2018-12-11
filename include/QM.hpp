
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

int variables = 0;	//кол-во переменных

class BooleanFunction 
{
private:

	struct Term
	{
		int glpos;				//позиция склеивания
		std::string tset;			// Битовая запись импликанта 
		int ones;		//Количество единиц 
		bool isused;		//Использовалась ли импликанта в склейке 
		int nimplicant;    //Номер набора  
		std::string nameofterm;		//Название терма 
		std::vector<int> numofterms;		//Номера термов 
		bool operator<(const Term& a) const //переопределение оператора сравнения для количества единиц у импликант 
		{
			return ones < a.ones; //сравниваются кол-ва единиц 
		}
		Term(int imp = 0, std::vector<int> tvec = std::vector<int>(), std::string str = "", int gl = 0, bool use = false)
			: nimplicant(imp), glpos(gl), ones(0), isused(use)
		{
			if (str == "")  
				nameofterm = 'n' + std::to_string(imp); // задаем новое значение из номера терма 
			else nameofterm = str;   
			if (tvec.empty()) numofterms.push_back(imp);  //если список термов пуст, инициализируем номером терма 
			else numofterms = tvec;
			int count = 1;		
			count <<= variables;  //сдвигаем счетчик на кол-во переменных 
			while (count >>= 1)
				if (gl & count)	//Если в позиции есть склейка, записываем *
					tset += '*';
				else if (imp & count)
				{
					tset += '1'; ++ones;
				}
				else tset += '0';
		}

	};

	std::vector<int> imnumbers; //Вектор номеров импликант
	std::vector<Term> implicants; //Вектор значений импликант 
	bool notone = false, notnul = false;

	public:
		std::string QuineMcClaskey(std::string str); 
		int count1(size_t x);
};

int BooleanFunction::count1(size_t x)	//Считает количество единиц 
{
	int numof1 = 0;
	while (x) 
	{ 
		numof1 += x % 2; x >>= 1; 
	} 
	return numof1;
}

std::string BooleanFunction::QuineMcClaskey(std::string str) 
{
	std::string answer; int combs = 1 << variables;	// побитово сдвигаем на количество переменных 
	for	(size_t i=0; i<str.size(); ++i) 
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

	std::vector<Term> temp; 
	std::vector<Term> finals;
	while (implicants.size() > 1) //Пока все возможные импликанты не будут склеены  
	{
		for (size_t i = 0; i < implicants.size() - 1; ++i) // Если импликанты повторяются, удаляем 
			for (size_t j = implicants.size() - 1; j > i; --j)
				if (implicants[j].tset == implicants[i].tset)
					implicants.erase(implicants.begin() + j);
		temp.clear();
		for (size_t i = 0; i < implicants.size() - 1; ++i) 
			for (size_t j = i + 1; j < implicants.size(); ++j)
				if (implicants[j].ones == implicants[i].ones + 1 && // Если мощности импликантов отличаются на 1
					implicants[j].glpos == implicants[i].glpos &&			// и у них стоят * в одних и тех же местах
					count1(implicants[i].nimplicant ^implicants[j].nimplicant) == 1)  // и расстояние хэмминга =1
				{
					implicants[i].isused = true; // помечаем импликанты склеенными 
					implicants[j].isused = true;
					std::vector<int> tempnum = implicants[i].numofterms;
					tempnum.insert(tempnum.end(), implicants[j].numofterms.begin(), implicants[j].numofterms.end());//собираем номера термов
					temp.push_back(
						Term(implicants[i].nimplicant, tempnum, // создаем новый импликант, склеивая старые, 
						implicants[i].nameofterm + implicants[j].nameofterm,		//закидываем номер импликанта, список термов, присваиваем название склеенного терма
						(implicants[i].nimplicant ^ implicants[j].nimplicant) | implicants[i].glpos));  // и позиции, на которых произошла склейка 
				}
		for (size_t i = 0; i < implicants.size(); ++i)
			if (!implicants[i].isused) finals.push_back(implicants[i]); //Все импликанты, которые нельзя склеить, запишем в первичные
		implicants = temp;		//Продолжим работу со склеенными импликантами 
		sort(implicants.begin(), implicants.end());
	}
	for (size_t i = 0; i < implicants.size(); ++i)
		finals.push_back(implicants[i]); //закидываем склеенные импликанты 

	bool **table = new bool *[finals.size()]; //создаем таблицу покрытия термов 
	for (size_t i = 0; i < finals.size(); i++) //размера кол-ва финальных термов 
		table[i] = new bool[imnumbers.size()];  // на кол-во единиц в функции 
	for (size_t i = 0; i < finals.size(); ++i)  
		for (size_t j = 0; j < imnumbers.size(); ++j)
			table[i][j] = false;
	for (size_t i = 0; i < finals.size(); ++i)
		for (size_t j = 0; j < finals[i].numofterms.size(); ++j)
			for (size_t k = 0; k < imnumbers.size(); ++k) // Заполняем таблицу 
				if (finals[i].numofterms[j] == imnumbers[k]) //Если первичный импликант покрывается, ставим 1
					table[i][k] = true;
	std::vector<size_t> tabl1, tabl2; 
	for (size_t i = 0; i < finals.size(); ++i)
		if (table[i][0]) tabl1.push_back(1 << i); //если импликант покрывается, записываем 1 на позицию номера покрываемой строки
	std::vector<size_t> res;
	for (size_t k = 1; k < imnumbers.size(); ++k)  //Для каждого первичного импликанта 
	{
		tabl2.clear();
		for (size_t i = 0; i < finals.size(); ++i)
			if (table[i][k])
				tabl2.push_back(1 << i); //если значение не пусто, записываем 1, на позицию номера покрываемой строки
		
		for (size_t i = 0; i < tabl1.size(); ++i)
			for (size_t j = 0; j < tabl2.size(); ++j)
				res.push_back(tabl1[i] | tabl2[j]); //записывем сложение двух покрытий   
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
		tabl1 = res;
		res.clear();
	}
	int min = count1(tabl1[0]); //Количество импликантов, входящих в покрытие 
	size_t index = 0;
	for (size_t i = 1; i < tabl1.size(); ++i)
		if (min > count1(tabl1[i]))
		{
			min = count1(tabl1[i]);
			index = i;
		}
	bool sum = false;
	answer = "F = "; // Выводится минимальная ДНФ
	for (size_t i = 0; i < finals.size(); ++i)
	{
		if (tabl1[index] & (1 << i))
		{
			if (sum)
				answer += " + ";
			sum = true;
			int neg = 1;
			neg <<= variables;
			int	count = 1;
			while (neg >>= 1)
			{
				if (!(finals[i].glpos & neg))
				{
					if (finals[i].nimplicant & neg)
						answer += "";
					else answer += "!";
					answer += "x" + std::to_string(count); // Выписываем переменные 
				}
				++count;
			}
		}
	}

	return answer;
}
