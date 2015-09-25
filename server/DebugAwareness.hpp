#ifndef DEBUGAWARENESS_HPP
#define DEBUGAWARENESS_HPP

#ifdef NDEBUG
    #ifndef NICYDEBUG
    #define NICYDEBUG
    #endif
    #ifndef NTHREADINGDEBUG
    #define NTHREADINGDEBUG
    #endif
#endif

#endif // DEBUGAWARENESS_HPP
