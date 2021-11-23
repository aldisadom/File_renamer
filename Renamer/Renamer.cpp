// Renamer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include<fstream>
#include<sstream>
#include <string>
#include <windows.h>
#include <locale>
#include <algorithm>
#include <vector>
#include <CommCtrl.h>
#include <filesystem>
#include <wchar.h>
#include <codecvt>


#include <filesystem> // C++17 standard header file name
#include <experimental/filesystem> // Header file for pre-standard implementation

using namespace std::experimental::filesystem;

using namespace std;

struct map_str
{
	wstring from;
	wstring to;
};

struct parameters_str
{
	bool diagram_clear = false;
	bool search_in_folders = false;
	bool filter_cb_alg = false;
	bool locale_UTF16 = false;
	bool filter_slash_quotes = false;
	bool full_name = false;
	wstring extensions = L"";
};

wstring parametrai_string [] =
{
	L"diagram_clear",
	L"extensions",
	L"search_in_folders",
	L"filter_cb_alg",
	L"filter_slash_quotes",
	L"full_name"
};

parameters_str params;
vector <map_str> maper;
vector <wstring> files_check;

void check_locale(wstring ligne)
{
	wstring compar = L"aa";
	compar[0] = 254;
	compar[1] = 255;
	int result1 = ligne.find(compar, 0);
	compar[0] = 255;
	compar[1] = 254;
	int result2 = ligne.find(compar, 0);

	if (result1 >= 0)
		params.locale_UTF16 = true;
	else if (result2 >= 0)
		params.locale_UTF16 = true;	
	else
		params.locale_UTF16 = false;

}

int cfg_puts(wstring tekstas, struct parameters_str *pars, int comment_pos)
{
	long value;
	int fStringMatch;
	int stringo_nr = 0;
	int count = 0;
	wstring tmp_text = L"";
	wstring parametras = L"";
	wstring value_text = L"";
	int separator_nr = 0;
	int lenght_value_text = 0;

	//geting which parameter it is read
	separator_nr = tekstas.find(L"=", 0);

	parametras = tekstas.substr(0, separator_nr);

	if (comment_pos > 0)
		lenght_value_text = comment_pos - separator_nr - 2;
	else
		lenght_value_text = tekstas.length() - separator_nr - 1;

	value_text = tekstas.substr(separator_nr + 1, lenght_value_text);

	if (parametras.empty())
	{
		cout << "Empty parameter\n";
		return 1;
	}
	//get parameter value

	if (value_text.empty())
	{
		cout << "Empty value\n";
		return 1;
	}

	// matching parameter whith parameter strings
	count = sizeof parametrai_string / sizeof parametrai_string[0];
	for (int i = 0; i < count; i++)
	{
		fStringMatch = parametras.find(parametrai_string[i]);
		if (fStringMatch == 0)
		{
			value = _wtol(value_text.c_str());
			stringo_nr = i;
			break;
		}
	}

	if (fStringMatch != 0)
	{
		cout << "Bad parameter\n";
		return 1;
	}
	else
	{
		// putting parameter value to structure
		switch (stringo_nr)
		{
		case 0:
			pars->diagram_clear = value;
			break;
		case 1:
			pars->extensions = value_text;
			break;
		case 2:
			pars->search_in_folders = value;
			break;
		case 3:
			pars->filter_cb_alg = value;
			break;		
		case 4:
			pars->filter_slash_quotes = value;
			break;
		case 5:
			pars->full_name = value;
			break;
		}
	}
	return 0;
}

int cfg_reads(struct parameters_str *params)
{
	int par_klaida;

	std::wifstream fichier("_cfg.txt");

	if (fichier)
	{
		std::wstring ligne;
		while (std::getline(fichier, ligne))
		{
			int result = ligne.find(L"#");
			if (result != 0)
				par_klaida = cfg_puts(ligne, params, result);
		}
	}
	else
	{
		cout << "No cfg file!\n";
		return 1;
	}
	fichier.close();

	return 0;
}

int map_reads()
{
	std::wifstream fichier("_map.txt");
	if (fichier)
	{
		int i = 0;
		int separator_nr = 0;

		std::wstring ligne;
		while (std::getline(fichier, ligne))
		{
			maper.resize(i + 1);
			separator_nr = ligne.find(L"\t", 0);
			maper[i].from = ligne.substr(0, separator_nr);
			maper[i].to = ligne.substr(separator_nr + 1, ligne.length() - separator_nr - 1);
			i++;			
		}
	}
	else
	{
		cout << "No map file!\n";
		return 1;
	}
	fichier.close();

	return 0;
}

void find_files()
{
	int i = 0;	
	string path = "./";
	wstring tekstas;
	int result = 0;
	if (params.search_in_folders)
	{
		for (const auto & entry : recursive_directory_iterator(path))
		{
			tekstas = entry.path();
			result = tekstas.find(params.extensions);
			if (result > 0)
			{
				files_check.resize(i + 1);
				files_check[i] = tekstas;
				i++;
			}
		}
	}
	else
	{
		for (const auto & entry : directory_iterator(path))
		{
			tekstas = entry.path();
			result = tekstas.find(params.extensions);
			if (result > 0)
			{
				files_check.resize(i + 1);
				files_check[i] = tekstas;
				i++;
			}
		}
	}

}

void diagram_clear()
{
	wstring file_name;
	wstring quatation = L"\"\"";
	long max_count = files_check.size();
	for (int i = 0; i < max_count; i++)
	{
		cout << i<<" cleared of "<<max_count<<"\n";
		file_name = files_check[i];
		std::wofstream fichier(file_name.c_str());
		if (fichier)
		{
			int separator_nr = 0;

			std::wstring ligne;
			ligne = L"DIAGRAM MAIN 0 0 0 200 200 black ZOOMABLE 0 0 0 200 200 1 DEFAULT_POSITION DEFAULT_SIZE ";
			ligne.append(quatation);
			fichier.write(ligne.c_str(), ligne.size());
		}
		fichier.close();
	}
}

int copy_files(wstring copy_name, wstring paste_name)
{
	std::wifstream fichier(copy_name.c_str(), std::ios::binary);

	wstring ligne;
	if (fichier.is_open())
	{
		std::wofstream temp_file(paste_name.c_str(), std::ios::binary);

		if (params.locale_UTF16)
		{
			std::locale utf16_locale(temp_file.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::generate_header>);
			temp_file.imbue(utf16_locale);
			fichier.imbue(std::locale(fichier.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
		}
		while (std::getline(fichier, ligne))
		{
			ligne.append(L"\n");
			temp_file << ligne;
		}
		temp_file.close();
	}
	else
		return 1;
	fichier.close();
	return 0;
}

void diagram_rename()
{
	wstring file_name;
	wstring temp_name = L"tmp.txt";
	wstring ligne;
	int result = 0;
	long j = 0;
	int number = 0;

	long max_count = files_check.size();
	bool pass = false;
	bool result_get = 0;

	for (int i = 0; i < max_count; i++)
	{
		cout << i << " renamed of " << max_count << "\n";

		file_name = files_check[i];
		file_name = file_name.substr(2, file_name.size() - 2);

		std::wifstream fichier(file_name.c_str(), std::ios::binary);

		if (i > 7)
		{
			i = i;
		}

		if (fichier.is_open())
		{
			int z = 0;
			std::wofstream temp_file(temp_name.c_str(), std::ios::binary);

			std::getline(fichier, ligne);
			check_locale(ligne);

			fichier.close();
			std::wifstream fichier(file_name.c_str(), std::ios::binary);
			if (params.locale_UTF16)
			{
				std::locale utf16_locale(temp_file.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::generate_header>);
				temp_file.imbue(utf16_locale);
				fichier.imbue(std::locale(fichier.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
			}
			
			while (std::getline(fichier, ligne))
			{
				pass = !params.filter_cb_alg && !params.filter_slash_quotes;
				j = 0;

				if (z > 339)
				{
					z = z;
				}
				if (!pass)
				{
					if (params.filter_cb_alg)
					{
						result = ligne.find(L"cb-alg-", number);
						if (result >= 0)
							pass = true;
						result = ligne.find(L"cb-sheet", number);
						if (result >= 0)
							pass = true;						
					}
					if (params.filter_slash_quotes)
					{
						result = ligne.find(L"\"", number);
						if (result >= 0)
							pass = true;
						result = ligne.find(L"\\", number);
						if (result >= 0)
							pass = true;
					}
				}

				if (pass)
				{
					for (j = 0; j < maper.size(); j++)
					{
						number = 0;
						result = 0;

						while (result >= 0)
						{
							result = ligne.find(maper[j].from, number);
							if (result < 0)
								break;
							if (params.full_name)
							{
								pass = false;
								if ((result > 0) & ((result + maper[j].to.size()) <= ligne.size()))
								{
									wstring texts;
									texts = ligne.substr(result - 1, 1);
									if (texts.compare(L"\"") == 0)
										pass = true;
									if (texts.compare(L"\\") == 0)
										pass = true;
									if (pass)
									{
										texts = ligne.substr(result + maper[j].from.size(), 1);
										pass = false;
										if (texts.compare(L"\"") == 0)
											pass = true;
										if (texts.compare(L"\\") == 0)
											pass = true;
										if (texts.compare(L"[") == 0)
											pass = true;										
									}
									if (!pass)
										break;
								}
								else
									break;
							}
							ligne.replace(result, maper[j].from.size(), maper[j].to);
							number = result + maper[j].to.size();
						}
					}
				}

				ligne.append(L"\n");
				//temp_file.write(ligne.c_str(), ligne.size());
				temp_file << ligne;
				z++;
//				cout << z << " line \n";
			}
			temp_file.close();
			copy_files(temp_name, file_name);
		}
		fichier.close();

		std::ifstream  src("tmp.txt", std::ios::binary);
		std::ofstream  dst(file_name.c_str(), std::ios::binary);

		dst << src.rdbuf();
		dst.close();
		src.close();
	}
}

int main()
{
	if (cfg_reads(&params) != 0)
		return 1;

	find_files();

	if (params.diagram_clear)
	{
		diagram_clear();
	}
	else
	{
		if (map_reads() != 0)
			return 1;

		diagram_rename();
	}

	int a = 0;
	a++;
}