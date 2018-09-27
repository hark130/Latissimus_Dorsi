#include <iostream>								// cout
#include <string>								// string

using namespace std;

int main(void)
{
	string str1 = "One";
	string str2 = "Two";

	cout << "String 1 len:\t" << str1.length() << endl;
	cout << "String 2 size:\t" << str2.size() << endl;
	cout << "String 1:\t" << str1 << endl;
	cout << "String 2:\t" << str2 << endl;
	if (str1 == str2)
	{
		cout << "String 1 and String 2 are equal" << endl;
	}
	else
	{
		cout << "String 1 and String 2 are NOT equal" << endl;
	}

	return 0;
}
