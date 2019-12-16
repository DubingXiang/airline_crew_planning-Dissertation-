/* Copyright (c) Robert Walker, support@tunesmithy.co.uk
 * Free source. Do what you wish with it - treat it as you would
 * example code in a book on c programming.
 */ 

/* Note
 * When you include this header then all your time_t variables will become
 * __int64 variables.
 *
 * Be sure to check that routines that use them work with __int64 variables.
 * By way of example, if you use sprintf, fprintf, sscanf, fscanf etc 
 * then you won't be able to use the %d format field for these variables
 * in a 32 bit build - they will overflow and be used for the next format field
 * as well. Instead convert your variables to unsigned ints and use %lu
 *
 * e.g. 
 * sprintf(sz,"time_saved %lu secs",(UINT)time(NULL));
 *
 * {
 *  time_t time_saved=0;
 *  UINT utime_saved=0;
 *  sscanf( sz, "%*s%lu", &utime_saved);
 *   // read it in as an unsigned long as sscanf can't read to __int64 variables 
 *   // directly
 *   time_saved=utime_saved;
 * }
 *
 
 * Unsigned ints are fine for dates until beyond the end of this century
 *
 * If your compiler has some other type of native 64 bit type then
 * change the line typedef __int64 t64 
 */

#include <time.h>

/*
 * Note. I think the usual time functions aren't thread safe becuase they use
 * a single struct for localtime, gmtime and mktime. Several threads
 * might access that same structure simultaneously.
 *
 * However we can make this implementation thread safe using thread local
 * variables if we so wish - simply define the tm struct that they
 * return to be thread local. But - I'm not sure if the pointer you get
 * back from localtime can be kept for any length of time with thread
 * local variables. Can the address for it change?? Anyone know?
 *
 * You can't use this method with DLLs that might be loaded using LoadLibrary
 * -  see
 * http://msdn.microsoft.com/library/en-us/dllproc/base/dynamic_link_library_data.asp
 */

#ifdef __cplusplus
extern "C" {
#endif
	// #define USE_THREAD_LOCAL_VARIABLES


	typedef long long t64;
	//#define time_t t64
	t64 time_64(t64 *pt);
	double difftime_64(t64 time1, t64 time0);
	t64 mktime_64(struct tm *today);
	struct tm *gmtime_64(t64 t);
	struct tm *localtime_64(t64 pt);
	void gmtime_64_s(t64 t, struct tm* m);

#ifdef __cplusplus
}
#endif