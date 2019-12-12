#include "grammar.h"

void Grammar::clear() {
	grammar.clear();
}

void Grammar::Iteration() {
	string tempRule = grammar;
	for (int i = 1; i <= level; i++) {
		int curlen = tempRule.length();
		int j = 0;
		while (j < curlen) {
			if (tempRule[j] == 'X') {
				rule += grammar;
				j++;
			}
			else {
				rule += tempRule[j];
				j++;
			}
		}
		tempRule = rule;
		rule.clear();
	}
	rule = tempRule;
	cout << rule << endl;
}

void Grammar::setGrammarName(const string& ref) {
	Gname = ref;
}

void Grammar::setLevel(int num) {
	level = num;
}

string Grammar::getGrammarName() {
	return Gname;
}

int Grammar::getLevel() {
	return level;
}

string Grammar::getRule() {
	return rule;
}