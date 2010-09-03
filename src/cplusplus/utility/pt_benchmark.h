/* -*-fill-column:100; coding:latin-1-unix;-*-
 *
 * pt_benchmark.h
 *
 * VENUE
 *
 * Portable high-resolution/real-time timers.
 *
 * Portable method of accessing CPU time and realtime for the given process.
 *
 * ISO C++'s time-measurement facilities are limited.   The functions `time', `ctime' and `clock' do
 * not measure in  microseconds, or nanoseconds.  For such facilities the  POSIX libraries or system
 * libraries are required.  POSIX is a portable/reliable standard that extends ISO C/C++.
 *
 * The header  defines class  pt::benchmark; objects  from this class  work like  stopwatches.  Each
 * object  provides the  wallclock time  (at nanoseconds  resolution, if  possible)  and per-process
 * kernel/user CPU times
 *
 * <pt_benchmark.h> is a  "standalone header" that requires no implementation  module.  Just copy it
 * to your project.  On some Linux systems, and on Sun Solaris, you may need to link in the realtime
 * library (<librt.a> or  <librt.so>) in order to use  this code.  Pass -lrt on  the gcc/g++ command
 * line.
 *
 ********
 * $Author: Andreas Spindler$
 * $Companion: winstd.h$
 * $Writestamp: 2009-05-28 17:10:36 spindlea$
 * $Maintained at: www.visualco.de$
 * $License: GPL$
 * $Compile: gcc -DDEBUG -Wall -Wextra -pedantic -fsyntax-only -x c++ pt_benchmark.h$
 *
 * Please send patches and suggestion to the author.
 *
 * Copyright 1998-2009
 *
 * This header is distributed standalone, and as part of the Phenotypes library.  Permission to use,
 * copy, modify, and distribute this software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE  IS PROVIDED "AS  IS" AND THE  AUTHOR DISCLAIMS ALL  WARRANTIES WITH REGARD  TO THIS
 * SOFTWARE INCLUDING ALL  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN  NO EVENT SHALL THE
 * AUTHOR  BE LIABLE FOR  ANY SPECIAL,  DIRECT, INDIRECT,  OR CONSEQUENTIAL  DAMAGES OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR  PROFITS, WHETHER  IN AN  ACTION OF  CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
#ifndef __PT_BENCHMARK_H
#define __PT_BENCHMARK_H

#ifndef __cplusplus
#error this is a C++ header
#endif /* __cplusplus */

/***********************************************************************
 *
 * PORTABILITY
 *
 ***********************************************************************/

/* Set target-OS macros, then include system headers. */
#ifndef PT_PORTABLE
#	if defined(_WINDOWS_)||defined(_WIN64)||defined(_WIN32)
#		define PT_WINNT			1										// Windows 32/64
#	elif defined(__APPLE__)
#		define PT_MACOS			1										// MacOS
#	elif defined(__CYGWIN__)||defined(__CYGWIN32__)||defined(__MINGW32__)
#		define PT_CYGWIN		1										// Cygwin
#	elif defined(sparc)||defined(__sparc)||defined(__sparc__)
#		define PT_SOLARIS		1
#	elif defined(_AIX)
#		define PT_AIX			1
#	elif defined(__linux__)||defined(unix)||defined(__FreeBSD__)||defined(__GNUC__)
#		define PT_GNU			1								// Linux, FreeBSD or some other UNIX
#	endif
#endif // PT_PORTABLE

#ifdef _MSC_VER
#pragma warning(disable: 4786)							// identifier truncated in debug information
#pragma warning(disable: 4800)							  // forcing value to bool 'true' or 'false'
#pragma comment(lib, "kernel32.lib")
#endif // _MSC_VER

#include <cassert>
#include <ctime>
#include <climits>
#include <cmath>
#if PT_WINNT															// Windows API
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif PT_MACOS					// uses CoreServices framework (see below)
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#else  // assume POSIX timers
#include <unistd.h>
#include <sys/times.h>
#endif

/* Set `PT_POSIX'.  The headers included above dissolved `_GNU_SOURCE' (if defined).  Therefore, we
   only test `_POSIX_C_SOURCE', `_POSIX_SOURCE' and `_BSD_SOURCE'. */
#ifndef PT_PORTABLE
#	define PT_PORTABLE_BENCHMARK
#	if _POSIX_C_SOURCE
#		define PT_POSIX			_POSIX_C_SOURCE
#	elif _POSIX_SOURCE
#		define PT_POSIX			1
#	elif _BSD_SOURCE
#		define _POSIX_C_SOURCE	1999506								  // imply this POSIX API on BSD
#		define PT_POSIX			_POSIX_C_SOURCE
#	else
#		ifdef PT_WINNT
			/* Window has native, incomplete POSIX/UNIX-subsystem: Interix or SFU (Services for
			   UNIX).  The latest version is SFU 3.5 from 2006. It is only part of Windows Server,
			   but is provided by Microsoft for download. See
			   <http://technet.microsoft.com/en-us/interopmigration/bb380242.aspx>.  */
#		elif defined(PT_MACOS)||defined(PT_CYGWIN)||defined(PT_SOLARIS)||defined(PT_AIX)||defined(PT_GNU)
#			define PT_POSIX		1999506	   // imply this POSIX API (in absence of `_POSIX_C_SOURCE')
#		else  // have only rudimentary ANSI-C!
#		endif
#	endif
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
#else																	// assume gcc
#if 0
#ifndef LLONG_MAX														// defined in <limits.h>
#error no 64-bit integer type available
#endif
typedef unsigned long long uint64_t;		 // C99 requires `long long' to be at least 64 bits wide
typedef signed long long int64_t;								   //  ...but permits it to be wider
#endif
#endif

/***********************************************************************
 *
 * IMPLEMENTATION
 *
 ***********************************************************************/

namespace pt
{
	/**
	 * @short Per-process kernel/user CPU times
	 *
	 * Exports  `clock_type' integer  type.   The constants  returned  by `clocks_per_second()'  and
	 * `clocks_per_millisecond()' give the actual resolution of ticks on this machine.
	 *
	 * `kernel_clocks()' returns  the processor time  the system has  used on behalf of  the calling
	 * process, until now.
	 *
	 * `user_clocks()' returns  the total processor time  the calling process has  used in executing
	 * the instructions of its program, until now, and from all CPU cores in multithreaded programs.
	 *
	 * @see http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/times.h.html
	 *
	 * @see `FileTimeToSystemTime',
	 * http://www.codeproject.com/KB/threads/getprocesstimes.aspx?df=100&forumid=15846&exp=0&select=799612.
	 */
	struct kernel_time
	{
#if PT_WINNT
		typedef int64_t clock_type;
		union { FILETIME ft; clock_type tcks; } _ktm, _utm;
		const clock_type kernel_clocks() const { return _ktm.tcks; }		// system (kernel) time
		const clock_type user_clocks() const { return _utm.tcks; }		// user-land time
		clock_type &kernel_clocks() { return _ktm.tcks; }
		clock_type &user_clocks() { return _utm.tcks; }
		static const clock_type clocks_per_second() {
			return 10000000U;				// `GetProcessTimes' returns 100-nanosecond-units -> 1e7
		}
		void reset() {
			FILETIME dummy[2];
			::GetProcessTimes(GetCurrentProcess(), &dummy[0], &dummy[1], &_ktm.ft, &_utm.ft);
		}
#elif PT_POSIX
		typedef clock_t clock_type;
		struct tms _kutm;
		clock_type _rtm;
		const clock_type kernel_clocks() const { return _kutm.tms_stime; }
		const clock_type user_clocks() const { return _kutm.tms_utime; }
		clock_type &kernel_clocks() { return _kutm.tms_stime; }
		clock_type &user_clocks() { return _kutm.tms_utime; }
		static const clock_type clocks_per_second() {
			/* Resulting constant scales `clock_t' values returned from the POSIX `times' function. */
			static const clock_t r = ::sysconf(_SC_CLK_TCK);
			return r;
		}
		void reset() { _rtm = ::times(&_kutm); }
#else // ANSI-C -> no CPU time, use wallclock time
#ifdef __GNUC__
#warning target OS no known: "pt::kernel_time" uses dummy ANSI-C implementation
#endif /* __GNUC__ */
		typedef clock_t clock_type;
		clock_type _dummy;
		const clock_type kernel_clocks() const { return 0; }
		const clock_type user_clocks() const { return 0; }
		clock_type &kernel_clocks() { return _dummy; }
		clock_type &user_clocks() { return _dummy; }
		static const clock_type clocks_per_second() { return CLOCKS_PER_SECOND; }
		void reset() {}
#endif
	
		static const double clocks_per_millisecond() {
			return static_cast<double>(clocks_per_second()) / 1000;
		}
	};

	/**
	 * @short The real time
	 *
	 * The real time is the time that passes outside off the machine.  It is measured in the machine
	 * by a  hardware-dependent resolution.  The resolution  cannot be set by  a particular process.
	 *
	 * On modern GHz processors, the 60 and 100 ticks per second resolution of the default CPU timer
	 * is inadequate  for serious profiling.   For example, a  machine must have  at least 1  GHz to
	 * provide nanoseconds (100 MHz  = 10 ns, 2 GHz = 1/2 ns).  Therefore  machines with more than 1
	 * GHz should have at least nanosecond-accuracy.  In reality it may be coarser than nanoseconds.
	 *
	 * RESETING THE TIMER
	 *
	 * `wallclock::reset()' loads a new  time as the time elapsed since the  system was last booted.
	 * The cost calling this function is (very) low.  The cost is three CPU cycles or even less!  On
	 * most systems  the call is independent  of system load  and other performance issues  that may
	 * affect `time()', `clock()', or `gettimeofday()'.
	 *
	 * When possible, for example,  `wallclock::reset()' uses the `gethrtime()' or `clock_gettime()'
	 * POSIX functions.  Frequent `gethrtime' calls  during profiling or testing have minimal effect
	 * on the performance of the process being profiled.
	 *
	 * CONSTANTS AND STATIC FUNCTIONS
	 *
	 * An adequate type to store high-precision  time values is exported as `clock_type'.  Note that
	 * the following assumption holds true:
	 *
	 *		assert(sizeof(kernel_time::clock_type) >= sizeof(wallclock::clock_type))
	 *
	 * Time values are converted to seconds using the constant returned by `clocks_per_second()'.
	 *
	 * The    `clocks_per_nanosecond()'    method     returns    nanosecond-precision,    but    not
	 * nanosecond-accuracy; these are  different things.  On most PCs  nanosecond-accuracy cannot be
	 * achieved.
	 *
	 * The static `infinity()' method returns the maximum  possible time value.  In any case this is
	 * a  constant  which will  never  be  reached during  the  execution  of  the current  process.
	 * Therefore, we can use this value to designate the infinitely distant moment in time.
	 *
	 * @see http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/times.h.html
	 *
	 * @see `FileTimeToSystemTime',
	 * <http://www.codeproject.com/KB/threads/getprocesstimes.aspx?df=100&forumid=15846&exp=0&select=799612>.
	 */
	struct wallclock
	{
		/***************************************
		 * POSIX.4, Sun Solaris, AIX, HP-UX, RT-Linux et.al.
		 *
		 * The `gethrtime()' API is defined by the real-time POSIX standard POSIX.4.  Only few
		 * systems supported this standard.  `gethrtime()' which offers a (theoretical) nanosecond
		 * granularity.  Typically, the timer starts measuring time since the most recent system
		 * booting, but this is just some moment in the past.  It is not correlated in any way to
		 * the time of day.  `gethrtime()' returns the time elapsed since the system was last
		 * booted, not the time since the epoch.  For the latter see the `gettimeofday()' POSIX
		 * standard function.
		 *
		 * Typically `gethrtime' reads a number of nanoseconds from special CPU registers
		 * (e.g. RDTSC on x86 and x86-64), and so is very fast.  `gethrtime' is unreliable, since
		 * the clock speeds can vary and, should this be an SMP machine, the cycle counter value
		 * could vary between different processors on the system.  For example, two close calls to
		 * `gethrtime()' may return equal values.  But there are also guarantees:
		 *
		 * - Monotonicity.  The later of two consecutive `gethrtime' calls will not return a smaller
		 *   value.  
		 * 
		 * - Linearity.  A `gethrtime' call is independent of system load.  The cost of calling
		 *   `gethrtime' is low: three CPU cycles or even less.
		 *
		 * Under RTLinux use:
		 *		#define HAVE_GETHRTIME
		 *		#include <rtl_time.h>
		 *		#include <pt_benchmark.h>
		 */
#ifndef _PT_BENCHMARK_ANSI_C
#if defined(HAVE_GETHRTIME)||defined(PT_SOLARIS)||defined(PT_AIX)
#ifndef HAVE_GETHRTIME
#define HAVE_GETHRTIME 1
#endif
		typedef ::hrtime_t clock_type;								  // 64-bit signed integer value
		void reset() { _clks = ::gethrtime(); }
		static const double clocks_per_nanosecond() { return 1; }
		static const double clocks_per_millisecond() { return 1e3; } // 1 ms = 1e-3 s = 0.001 s
		static const clock_type clocks_per_second() { return 1000000000LL; } // 1 ns = 1e-9 s = 0.000000001 s
		//static const clock_type infinity() { return HRTIME_INFINITY; }
		static const clock_type infinity() { return UINT64_MAX; } // when HRTIME_INFINITY is n/a
		
		/***************************************
		 * POSIX.1-3, Linux, Cygwin etc.
		 *
		 * `_POSIX_TIMERS' is defined in <unistd.h> to a value greater than 0.  See also sysconf(3)
		 * and <http://linux.die.net/man/3/clock_gettime>.
		 */
#elif _POSIX_TIMERS > 0
		struct timespec _tsr;											// realtime
#if 0
#ifdef _POSIX_MONOTONIC_CLOCK
		struct timespec _tsm;											// monotonic
#endif
#ifdef _POSIX_CPUTIME
		struct timespec _tsp;											// process
#endif
#ifdef _POSIX_THREAD_CPUTIME
		struct timespec _tst;											// thread
#endif
#endif
		typedef long clock_type;										// alias timespec::tv_nsec
		void reset() {
			clock_gettime(CLOCK_REALTIME, &_tsr); 
			double nsecs = _tsr.tv_nsec * 1e-9;
			_clks = _tsr.tv_sec + static_cast<clock_type>(ceil(nsecs));
#if 0
#ifdef _POSIX_MONOTONIC_CLOCK
			clock_gettime(CLOCK_MONOTONIC, &_tsm);
#endif
#ifdef _POSIX_CPUTIME
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_tsp);
#endif
#ifdef _POSIX_THREAD_CPUTIME
			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &_tst);
#endif
#endif
		}
		static const double clocks_per_nanosecond() {
			timespec t;
			clock_getres(CLOCK_REALTIME, &t);  // t.tv_nsec -> length of a timer unit in nanoseconds
			double clk_rate = 1e9 / t.tv_nsec;							// clocks per second
			return clk_rate / 1e9;
		}
		static const double clocks_per_millisecond() { return clocks_per_nanosecond() * 1e3; }
		static const clock_type clocks_per_second() { 
			timespec t;
			clock_getres(CLOCK_REALTIME, &t);  // t.tv_nsec -> length of a timer unit in nanoseconds
			double clk_rate = 1e9 / t.tv_nsec;							// clocks per second
			return static_cast<clock_type>(ceil(clk_rate));
		}
		static const clock_type infinity() { return LONG_MAX; }
		
		/***************************************
		 * MacOS X without `_POSIX_TIMERS'
		 *
		 * We use CoreServices framework, see
		 * <http://developer.apple.com/qa/qa2004/qa1398.html>
		 */
#elif PT_MACOS
		typedef uint64_t clock_type;
		void reset() { _clks = mach_absolute_time(); }
		static const double clocks_per_nanosecond() { 
			static mach_timebase_info_data_t tbi;
			static double nsec_conversion_factor = 0;
			if (nsec_conversion_factor == 0) {
				kern_return_t err = mach_timebase_info(&tbi);
				if (err == 0)
					nsec_conversion_factor = double(tbi.numer) / double(tbi.denom);
			}
			return nsec_conversion_factor;
		}
		static const double clocks_per_millisecond() { return clocks_per_nanosecond() * 1e3; }
		static const clock_type clocks_per_second() { return static_cast<clock_type>(clocks_per_nanosecond() * 1e9); }
		static const clock_type infinity() { return UINT64_MAX; }
		
		/***************************************
		 * Windows NT
		 */
#elif PT_WINNT
		typedef LONGLONG clock_type;							  // 64-bit signed integer value
		void reset() { 
			LARGE_INTEGER i;
			::QueryPerformanceCounter(&i);
			_clks = i.QuadPart;
		}
		static const double clocks_per_nanosecond() {
			LARGE_INTEGER freq;
			if (QueryPerformanceFrequency(&freq) == FALSE)
				/* This hardware does not support high-res-timers.  This should only occur on
				   pre-Pentium machines.  We could use `GetTickCount', `GetSystemTimeAsFileTime',
				   `GetProcessTimes' or `GetThreadTimes' as an approximation here. */
				assert(0);
			double hz = static_cast<double>(freq.QuadPart);
			return hz * 1e-9;
		}
		static const double clocks_per_millisecond()
			{ return clocks_per_nanosecond() * 1e3; }
		static const clock_type clocks_per_second()
			{ return static_cast<clock_type>(clocks_per_nanosecond() * 1e9); }
		static const clock_type infinity()
			{ return static_cast<clock_type>(0x8000000000000000LL); // LONGLONG_MAX n/a
			}

		/***************************************
		 * ANSI-C
		 *
		 * Use `clock' and `CLOCKS_PER_SECOND'.
		 */
#else
#define _PT_BENCHMARK_ANSI_C
#endif
#endif
#ifdef _PT_BENCHMARK_ANSI_C
		typedef clock_t clock_type;
		void reset() { _clks = ::clock(); }
		static const double clocks_per_nanosecond() { return double(CLOCKS_PER_SEC) * 1e-9; }
		static const double clocks_per_millisecond() { return double(CLOCKS_PER_SEC) * 1e-3; }
		static const clock_type clocks_per_second() { return CLOCKS_PER_SEC; }
		static const clock_type infinity() { return sizeof(clock_type) == sizeof(int) ? UINT_MAX : ULONG_MAX; }
#endif
		
		clock_type _clks;
		const clock_type clocks() const { return _clks; }
		clock_type& clocks() { return _clks; }
	};
	/***/

	/**
	 * @short Measure CPU Process-Time Inquiry
	 *
	 * Portable  class to  compute a  process' consumption  of CPU-  and wallclock-time  between two
	 * events.
	 *
	 * Objects of this class  store process- and real times.  Process time  is the time this process
	 * inquired from this machine.   Real time (aka wallclock time) is the  time that passes outside
	 * the  process.  The  real  time therefore  contains time  the  systems spends  waiting on  I/O
	 * requests.
	 *
	 * Process time is divided into kernel- and user-time. The kernel time is the time a process has
	 * used by  calling system functions.   The user  time is the  total processor time  the calling
	 * process has used in executing the instructions  of its program.  Kernel and user times are on
	 * the behalf of the current process, i.e., without terminated child processes but including all
	 * threads.
	 *
	 * Note that  `benchmark' objects never store  the time of date  in any way.   They always store
	 * time differences measured by  the hardware and the OS. These are  neutral time amounts rather
	 * than time periods!
	 *
	 * This implementation has been tested under  Windows 2000/XP/XP64, Linux and Solaris, with MSVC
	 * and gcc.
	 *
	 * EXAMPLES
	 *
	 * To compute the difference between now and some earlier time, use:
	 *
	 *		benchmark diff = benchmark() - before;
	 *		benchmark diff = before.stop(); // same result
	 * 
	 * As in:
	 *
	 *		int main(int argc, const char* argv[])
	 *		{
	 *			benchmark bnm_main;
	 *				.
	 *				.
	 *			benchmark bnm_calc;
	 *			long_calculation();
	 *			bnm_calc -= benchmark();
	 *
	 *			// end of program
	 *			//	bnm_main: time inquired by the whole process
	 *			//	bnm_calc: time inquired by long_calculation()
	 *
	 *			bnm_main -= benchmark();
	 *
	 *			printf("runtimes total/calculation: real=%.3f/%.3f   system=%.3f/%.3f   user=%.3f/%.3f",
	 *				   bnm_main.real_seconds(), bnm_calc.real_seconds(),
	 *				   bnm_main.kernel_seconds(), bnm_calc.kernel_seconds(),
	 *				   bnm_main.user_seconds(), bnm_calc.user_seconds());
	 *		}
	 *
	 * `print_benchmark' and `print_benchmark_difference' are  convenience macros. Print the current
	 * value of a benchmark object `b':
	 *
	 *     print_benchmark("time of b", b);
	 *
	 * Print the difference to now and a benchmark object `b':
	 *
	 *     print_benchmark_difference("time passed since b", b);
	 *
	 * @see http://www.gnu.org/software/libtool/manual/libc/Processor-And-CPU-Time.html#Processor-And-CPU-Time
	 */
	class benchmark
	{
		/*** EXPORTED TYPES ***/
	public:
		typedef kernel_time::clock_type kernel_clock_type;
		typedef wallclock::clock_type real_clock_type;
	
		/*** CONSTRUCTION ***/
	public:
		/** 
		 * The default c'tor initializes an object from the current CPU times ("now").
		 */
		benchmark();
	private:
		benchmark(const kernel_clock_type k, const kernel_clock_type u, const real_clock_type r);
		/***/

		/** INTERFACE ***/
	public:
		/** 
		 * Return times as machine-related CPU time, milliseconds and seconds.
		 *
		 * `clocks' gives the elapsed real-time.
		 */
		const kernel_clock_type kernel_clocks() const { return _proctm.kernel_clocks(); }
		const kernel_clock_type user_clocks() const { return _proctm.user_clocks(); }
		const real_clock_type clocks() const { return _realtm.clocks(); }

		const double kernel_milliseconds() const {
			return static_cast<double>(kernel_clocks()) / kernel_time::clocks_per_millisecond();
		}
		const double user_milliseconds() const {
			return static_cast<double>(user_clocks()) / kernel_time::clocks_per_millisecond();
		}
		const double real_milliseconds() const {
			return static_cast<double>(clocks()) / wallclock::clocks_per_millisecond();
		}

		const double kernel_seconds() const {
			return static_cast<double>(kernel_clocks()) / kernel_time::clocks_per_second();
		}
		const double user_seconds() const {
			return static_cast<double>(user_clocks()) / kernel_time::clocks_per_second();
		}
		const double real_seconds() const {
			return static_cast<double>(clocks()) / wallclock::clocks_per_second();
		}
		/***/

		/** `stop' and `operator-=' returns a benchmark object that expresses the time difference to
			some other benchmark object.  The difference is positive when the other object was
			constructed later than this `benchmark' object; otherwise the result is negative.
		
			@example,
		
				void f() {
					benchmark bnm1, bnm2;
						.
						.
					bnm1 -= benchmark();		   // compute time difference to now
					bnm2 = bnm2.stop();									// dto.
				}

			@param other `benchmark' constructed later than this benchmark.  Defaults to current
						 time (now).
		*/
		const benchmark stop(const benchmark &other = benchmark()) const;
		const benchmark& operator -= (const benchmark &other);
		/***/
		
		friend const benchmark operator - (const benchmark &later, const benchmark &before);

		/*** IMPLEMENTATION ***/
	
		/*** ATTRIBUTES ***/
	private:
		kernel_time _proctm;
		wallclock _realtm;
	};

	/*************************************************************************
	 * 
	 * implementation of class benchmark
	 *
	 *************************************************************************/

	inline benchmark::benchmark()
	{
		_proctm.reset();							   // start the process clock (CPU/kernel timer)
		_realtm.reset();								 // start the nanoseconds clock (real timer)
	}

	inline benchmark::benchmark(const kernel_clock_type k, const kernel_clock_type u, const real_clock_type r)
	{
		_proctm.kernel_clocks() = k;
		_proctm.user_clocks() = u;
		_realtm.clocks() = r;
	}

	inline const benchmark& benchmark::operator -= (const benchmark &later)
	{
		return *this = stop(later);
	}

	inline const benchmark benchmark::stop(const benchmark& later) const
	{
		return benchmark(later.kernel_clocks() - kernel_clocks(),
						 later.user_clocks() - user_clocks(),
						 later.clocks() - clocks());
	}

	/** Global operator. Get time difference between now (`b') and some time before (`a'): to get a
		positive result `b' must have been constructed/assigned after `a' to describe some "later
		time". */

	inline const benchmark operator - (const benchmark &a, const benchmark &b)
	{
		return a.stop(b);
	}

	/** Print benchmark times on stdout (real/system/user).  Declared as a macro (not a function)
		because we do not want to include I/O-stream library headers.

		Example:

			#include <iostream>
			#include <iomanip>
			#include <pt_benchmark.h>

			int main(int argc, const char* argv[]) {
				pt::benchmark bnm_main;
					.
					.
				print_benchmark_difference(bnm_main);
			}	
	*/

//	required 12 wallclock secs ( 2.32 usr +  0.33 sys =  2.65 CPU)
	
#define print_benchmark_ratios() do {												\
		std::cout <<        "            " << std::fixed << std::setprecision(9)	\
				  <<   "wallclocks/sec = " << pt::wallclock::clocks_per_second()	\
				  << "       ticks/sec = " << pt::kernel_time::clocks_per_second()	\
				  << std::endl << std::flush;										\
	} while(0)

#define print_benchmark(bnm) do {																\
		std::cout << std::endl																	\
				  <<  "            " << std::fixed << std::setprecision(2)						\
				  <<    "real time " << bnm.real_seconds() << " secs, "							\
				  << "process time " << (bnm.kernel_seconds() + bnm.user_seconds()) << " secs"	\
				  <<             " (" << bnm.kernel_seconds() << " system"						\
				  <<            " + " << bnm.user_seconds() << " user)"							\
				  << std::endl << std::flush;													\
	} while(0)

#define print_benchmark_difference(bnm) do {							\
		const pt::benchmark bnmdiff = (bnm).stop();						\
		print_benchmark(bnmdiff);										\
	} while(0)
/***/
} // namespace pt
	
#endif /* __PT_BENCHMARK_H */
