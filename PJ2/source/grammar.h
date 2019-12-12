#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Grammar {
public:
	string	Gname;
	int		level = 3;
	string	grammar = "FA[*+&X][-/&X][/%X]B";
	string	rule;

	Grammar() {

	}
	void	clear();
	void	Iteration();
	void	setGrammarName(const string&);
	void	setLevel(int);
	string	getGrammarName();
	int		getLevel();
	string	getRule();
};