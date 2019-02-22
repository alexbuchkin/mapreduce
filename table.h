#ifndef TABLE
#define TABLE

#include <vector>
#include <string>

using namespace std;

template <class T>
struct table
{
	vector <T> vec;
	int put(const T&);
	int find(const T&) const;
	T operator[](int) const;
};

template <class T> inline int
table<T>::find(const T& t) const
{
	for (int i = 0; i < vec.size(); ++i) {
		if (vec[i] == t) {
			return i;
		}
	}
	return -1;
}

template <class T> inline int
table<T>::put(const T& t)
{
	int i;
	if ((i = find(t)) == -1) {
		vec.push_back(t);
		i = vec.size() - 1;
	}
	return i;
}

template <class T> T
table<T>::operator[](int n) const
{
	if (n < 0 || n >= vec.size()) {
		throw string("Table trying to access by index " + to_string(n) + ", but size of this table is " + to_string(vec.size())).c_str();
	}
	return vec[n];
}

#endif
