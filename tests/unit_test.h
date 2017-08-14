//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#ifndef _PAL_UNIT_TEST_H_
#define _PAL_UNIT_TEST_H_ 1

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>

// TODO: add ability to specify a list on the command line
class unit_test
{
	static constexpr const char *green = "\x1B[32m";
	static constexpr const char *red = "\x1B[31m";
	static constexpr const char *yellow = "\x1B[33m";
	static constexpr const char *reset = "\x1B[0m";

public:
	explicit unit_test( const std::string &n ) : _name( n ) {}

	/// add a function to test, taking a name.
	/// this is usually done with a lambda:
	/// foo["mytest"] = [&]() {};
	std::function<void(void)> &operator[]( const std::string &n )
	{
		_all_tests.emplace_back( std::make_pair(n,std::function<void(void)>()) );
		return _all_tests.back().second;
	}

	/// add a method to run at the end to clean up any temp files
	std::function<void(void)> &cleanup( void )
	{
		_cleanup.emplace_back();
		return _cleanup.back();
	}

	void add_to_run( std::string n ) { _to_run.emplace_back( std::move(n) ); }

	inline int run( bool quiet );

	/// for a long running process
	inline void start_run( const std::string &tag );
	inline void run_tick( const std::string &tag );
	inline void finish_run( const std::string &tag );

	inline void success( const std::string &tag, const std::string &msg );
	inline void negative_success( const std::string &tag, const std::string &msg );
	inline void failure( const std::string &tag, const std::string &msg );
	
	int get_number_failed( void ) const { return _fail; }
	int get_number_succeeded( void ) const { return _success; }
	int get_number_run( void ) const { return _success + _fail; }

private:
	std::string _name;
	std::vector<std::pair<std::string,std::function<void(void)>>> _all_tests;
	std::vector<std::function<void(void)>> _cleanup;
	std::vector<std::string> _to_run;
	std::string _running;
	int _success = 0;
	int _fail = 0;
	bool _doprint = true;
	int _tick = 0;
};

////////////////////////////////////////

inline int
unit_test::run( bool quiet )
{
	_doprint = ! quiet;
	if ( _doprint )
		std::cout << "Starting " << _name << " tests..." << std::endl;
	int totfail = 0;
	int totsuc = 0;
	for ( auto &t: _all_tests )
	{
		if ( ! _to_run.empty() )
		{
			bool inset = false;
			for ( auto tr = _to_run.begin(); tr != _to_run.end(); ++tr )
			{
				if ( (*tr) == t.first )
				{
					inset = true;
					_to_run.erase( tr );
					break;
				}
			}
			if ( ! inset )
				continue;
		}
		_running = t.first;
		_success = 0;
		_fail = 0;
		if ( _doprint )
			std::cout << "\n " << _running << ":" << std::endl;
		try
		{
			t.second();
		}
		catch ( std::exception &e )
		{
			if ( _doprint )
				std::cerr << red << "  ERROR" << reset << " - exception during test '" << _running << "': " << e.what() << std::endl;
			++_fail;
		}
		catch ( ... )
		{
			if ( _doprint )
				std::cerr << red << "  UNKNOWN ERROR" << reset << ": during test '" << _running << "'" << std::endl;
			++_fail;
		}
		if ( _doprint )
		{
			if ( _fail == 0 )
			{
				if ( _success == 0 )
					std::cout << yellow << "    NO TESTS ENABLED" << reset << std::endl;
				else
					std::cout << " " << _running << ": " << _success << " / " << _success << green << " passed" << reset << std::endl;
			}
			else
			{
				std::cout << red << " FAILURE " << reset << _running << ": (" << green << _success << reset << ", " << red << _fail << reset << ") / " << (_success+_fail) << std::endl;
			}
		}
		totsuc += _success;
		totfail += _fail;
	}

	if ( ! _to_run.empty() )
	{
		for ( auto &t: _to_run )
		{
			if ( _doprint )
				std::cerr << red << "ERROR" << reset << ": Unknown test '" << t << "' requested..." << std::endl;
			++totfail;
		}
	}

	_success = totsuc;
	_fail = totfail;

	if ( _doprint )
	{
		if ( get_number_run() == get_number_succeeded() )
		{
			std::cout << '\n' << green << "SUCCESS" << reset << ' ' << get_number_run() << " / " << get_number_run() << " passed." << std::endl;
		}
		else
		{
			std::cout << '\n' << red << "FAILURE " << reset << ": (" << green << _success << reset << ", " << red << _fail << reset << ") / " << (_success+_fail) << " passed" << std::endl;
		}
	}

	for ( auto c = _cleanup.rbegin(); c != _cleanup.rend(); ++c )
	{
		try
		{
			(*c)();
		}
		catch ( std::exception &e )
		{
			std::cerr << red << "ERROR" << reset << ": during cleanup: " << e.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << red << "UNKNOWN ERROR" << reset << ": during cleanup" << std::endl;
		}
	}
	return _fail;
}

////////////////////////////////////////

inline void
unit_test::start_run( const std::string &tag )
{
	if ( _doprint )
		std::cout << "  " << tag << ": " << std::flush;
}

////////////////////////////////////////

inline void unit_test::run_tick( const std::string &tag )
{
	static const char *tickle = "-\\|/-\\|/";
	if ( _doprint )
		std::cout << "\r  " << tag << ": " << tickle[(_tick++)&0x7] << std::flush;
}

////////////////////////////////////////

inline void unit_test::finish_run( const std::string &tag )
{
	if ( _doprint )
		std::cout << '\r';
}

////////////////////////////////////////

inline void
unit_test::success( const std::string &tag, const std::string &msg )
{
	++_success;
	if ( _doprint )
		std::cout << green << "  " << tag << reset << ": " << msg << std::endl;
}

////////////////////////////////////////

inline void
unit_test::negative_success( const std::string &tag, const std::string &msg )
{
	++_success;
	if ( _doprint )
		std::cout << green << "  " << tag << reset << " (expected fail): " << msg << std::endl;
}

////////////////////////////////////////

inline void
unit_test::failure( const std::string &tag, const std::string &msg )
{
	++_fail;
	if ( _doprint )
		std::cout << "  " << red << tag << reset << ": " << msg << std::endl;
}

#endif
