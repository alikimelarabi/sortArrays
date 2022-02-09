#include <iostream>
#include <algorithm>
#include <numeric>  // for std::accumulate
#include <vector>
#include <string>
#include <cstdlib>  // for std::rand
#include <cassert>

using namespace std;

//========================================================================
//  Set this to false to skip the insertion sort tests; you'd do this if
//  you're sorting so many items that insertion_sort would take more time
//  than you're willing to wait.

const bool TEST_INSERTION_SORT = true;

//========================================================================

//========================================================================
// Timer t;                 // create a timer
// t.start();               // start the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

#if __cplusplus >= 201103L  // C++11

#include <chrono>

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
	double elapsed() const
	{
		std::chrono::duration<double, std::milli> diff =
			std::chrono::high_resolution_clock::now() - m_time;
		return diff.count();
	}
private:
	std::chrono::high_resolution_clock::time_point m_time;
};

#elif defined(_MSC_VER)  // not C++11, but Windows

#include <windows.h>

class Timer
{
public:
	Timer()
	{
		QueryPerformanceFrequency(&ticksPerSecond);
		start();
	}
	void start()
	{
		QueryPerformanceCounter(&m_time);
	}
	double elapsed() const
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000.0 * (now.QuadPart - m_time.QuadPart)) / ticksPerSecond.QuadPart;
	}
private:
	LARGE_INTEGER m_time;
	LARGE_INTEGER ticksPerSecond;
};

#else // not C++11 or Windows, so C++98

#include <ctime>

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::clock();
	}
	double elapsed() const
	{
		return (1000.0 * (std::clock() - m_time)) / CLOCKS_PER_SEC;
	}
private:
	std::clock_t m_time;
};

#endif

//========================================================================

// Here's a class that is not cheap to copy because the objects contain
// a large array.

// We'll simplify writing our timing tests by declaring everything public
// in this class.  (We wouldn't make data public in a class intended for
// wider use.)

typedef int IdType;

const int NREADINGS = 150;

struct Sensor
{
	IdType id;
	double avg;
	double readings[NREADINGS];
	Sensor(IdType i) : id(i)
	{
		// create random sensor readings (from 0 to 99)
		for (size_t k = 0; k < NREADINGS; k++)
			readings[k] = rand() % 100;
		// (accumulate computes 0.0 + readings[0] + readings[1] + ...)
		// accumulate(First, Last, init_value)
		avg = accumulate(readings, readings + NREADINGS, 0.0) / NREADINGS;
	}
};

inline
bool compareSensor(const Sensor& lhs, const Sensor& rhs)
{
	// The Sensor with the higher average should come first.  If they have
	// the same average, then the Sensor with the smaller id number should
	// come first.  Return true iff lhs should come first.  Notice that
	// this means that a false return means EITHER that rhs should come
	// first, or there's a tie, so we don't care which comes first,

	if (lhs.avg > rhs.avg)
		return true;
	if (lhs.avg < rhs.avg)
		return false;
	return lhs.id < rhs.id;
}

inline
bool compareSensorPtr(const Sensor* lhs, const Sensor* rhs)
{
	// TODO: You implement this.  Using the same criteria as compareSensor,
	//       compare two Sensors POINTED TO by lhs and rhs.  Think about
	//       how you can do it in one line by calling compareSensor.

	// Just so this will compile, we'll pretend every comparison results in
	// a tie, so there's no preferred ordering.
	return compareSensor(*lhs, *rhs);
}

bool isSorted(const vector<Sensor>& s)
{
	// Return true iff the vector is sorted according to the ordering
	// relationship compareSensor

	for (size_t k = 1; k < s.size(); k++)
	{
		if (compareSensor(s[k], s[k - 1]))
			return false;
	}
	return true;
}

void insertion_sort(vector<Sensor>& s, bool comp(const Sensor&, const Sensor&))
{
	// TODO: Using the insertion sort algorithm (pp. 332-333), sort s
	//       according to the ordering relationship passed in as the
	//       parameter comp. There is also a version on the class website.

	// Note:  The insertion sort algorithm on pp. 312-313 of the Carrano
	// book 6th edition is incorrect; someone made a change from the 5th
	// edition and messed it up.  See the errata page entry for page 313 at
	// http://homepage.cs.uri.edu/~carrano/WMcpp6e

	// Just to show you how to use the second parameter, we'll write code
	// that sorts only a vector of 2 elements.  (This is *not* the
	// insertion sort algorithm.)

	// Note that if comp(x,y) is true, it means x must end up before y in the
	// final ordering.
	for (int i = 0; i < s.size(); i++)
	{
		for (int j = i; j > 0 && comp(s[j], s[j - 1]); j--)
		{
			swap(s[j], s[j - 1]);
		}
	}
}

// Report the results of a timing test

void report(string caption, double t, const vector<Sensor>& s)
{
	cout << t << " milliseconds; " << caption
		<< "; first few sensors are\n\t";
	size_t n = s.size();
	if (n > 5)
		n = 5;
	for (size_t k = 0; k < n; k++)
		cout << " (" << s[k].id << ", " << s[k].avg << ")";
	cout << endl;
}

void test(int testId) {
	int nsensors1 = 1000;
	int nsensors2 = 50;
	int nsensors3 = 500;
	int nsensors4 = 5000;

	// Create a random ordering of id numbers 0 through nsensors-1
	vector<IdType> ids1;
	for (size_t j = 0; j < nsensors1; j++)
		ids1.push_back(IdType(j));
	random_shuffle(ids1.begin(), ids1.end());  // from <algorithm>

											   // Create a bunch of Sensors
	vector<Sensor> unorderedSensors1000;
	for (size_t k = 0; k < ids1.size(); k++)
		unorderedSensors1000.push_back(Sensor(ids1[k]));

	/*--------------------------------------------------------------------------*/

	vector<IdType> ids2;
	for (size_t j = 0; j < nsensors2; j++)
		ids2.push_back(IdType(j));
	random_shuffle(ids2.begin(), ids2.end());  // from <algorithm>

											   // Create a bunch of Sensors
	vector<Sensor> unorderedSensors50;
	for (size_t k = 0; k < ids2.size(); k++)
		unorderedSensors50.push_back(Sensor(ids2[k]));
	/*--------------------------------------------------------------------------*/

	vector<IdType> ids3;
	for (size_t j = 0; j < nsensors3; j++)
		ids3.push_back(IdType(j));
	random_shuffle(ids3.begin(), ids3.end());  // from <algorithm>

											   // Create a bunch of Sensors
	vector<Sensor> unorderedSensors500;
	for (size_t k = 0; k < ids3.size(); k++)
		unorderedSensors500.push_back(Sensor(ids3[k]));
	// Create a timer

	/*--------------------------------------------------------------------------*/

	vector<IdType> ids4;
	for (size_t j = 0; j < nsensors4; j++)
		ids4.push_back(IdType(j));
	random_shuffle(ids4.begin(), ids4.end());  // from <algorithm>

											   // Create a bunch of Sensors
	vector<Sensor> unorderedSensors5000;
	for (size_t k = 0; k < ids4.size(); k++)
		unorderedSensors5000.push_back(Sensor(ids4[k]));
	// Create a timer

	Timer timer;

	// Sort the Sensors using the STL sort algorithm.  It uses a variant
	// of quicksort called introsort.

	switch (testId) {
	case 1: {
		vector<Sensor> sensors1(unorderedSensors1000);
		sort(sensors1.begin(), sensors1.end(), compareSensor);
		assert(isSorted(sensors1));
		break; }
	case 2: {
		vector<Sensor> sensors2(unorderedSensors50);
		insertion_sort(sensors2, compareSensor);
		assert(isSorted(sensors2));
		break; }
	case 3: {
		vector<Sensor> sensors3(unorderedSensors1000);
		insertion_sort(sensors3, compareSensor);
		assert(isSorted(sensors3));
		break;
	}
	case 4: {
		vector<Sensor> sensors1(unorderedSensors1000);
		sort(sensors1.begin(), sensors1.end(), compareSensor);
		insertion_sort(sensors1, compareSensor);
		assert(isSorted(sensors1));
		break;
	}
	case 5: {
		vector<Sensor> sensors4(unorderedSensors1000);
		timer.start();
		sort(sensors4.begin(), sensors4.end(), compareSensor);
		double elapsed1 = timer.elapsed();
		assert(isSorted(sensors4));

		vector<Sensor> sensors5(unorderedSensors1000);
		timer.start();
		insertion_sort(sensors5, compareSensor);
		double elapsed3 = timer.elapsed();
		assert(isSorted(sensors5));
		assert(elapsed1 < elapsed3);
	}
	case 6: {
		vector<Sensor> sensors5(unorderedSensors1000);
		timer.start();
		insertion_sort(sensors5, compareSensor);
		double elapsed3 = timer.elapsed();
		assert(isSorted(sensors5));

		vector<Sensor> sensors6(sensors5);
		timer.start();
		insertion_sort(sensors6, compareSensor);
		double elapsed4 = timer.elapsed();
		assert(isSorted(sensors6));
		assert(elapsed3 > elapsed4);
	}
	case 7:
	case 8:
	case 9:
	case 10: {
		vector<Sensor> sensors5(unorderedSensors500);
		timer.start();
		insertion_sort(sensors5, compareSensor);
		double elapsed3 = timer.elapsed();
		assert(isSorted(sensors5));

		vector<Sensor> sensors6(unorderedSensors5000);
		timer.start();
		insertion_sort(sensors6, compareSensor);
		double elapsed4 = timer.elapsed();
		assert(isSorted(sensors6));
		assert(elapsed3 * 10 < elapsed4);
		assert(elapsed3 * 1000 > elapsed4);
	}
			 break;
	default: {
		cout << "testId not in range" << endl;
		break;
	}
	}
}

int main()
{
	for (int i = 1; i <= 10; i++)
	{
		test(i);
		cout << "Test " << i << " passed!" << endl;
	}
	return 0;
}