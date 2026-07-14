#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <utility>
using namespace std;

extern int TESTS_RUN;
extern int TESTS_PASSED;
extern int TESTS_FAILED;
extern int TESTS_CRASH;

static void beginSuite(const string& name) {
    cout << "\n============================================================\n";
    cout << "  SUITE: " << name << "\n";
    cout << "============================================================\n";
}

// Simple pass/fail helpers
static void _pass(const char* label) {
    TESTS_PASSED++; TESTS_RUN++;
    cout << "    [PASS] " << label << "\n";
}
static void _fail(const char* label, const string& msg) {
    TESTS_FAILED++; TESTS_RUN++;
    cout << "    [FAIL] " << label << "\n" << "           " << msg << "\n";
}

// ASSERT_TRUE
#define ASSERT_TRUE(expr, label) do { try { \
    bool _r = (bool)(expr); \
    if (_r) _pass(label); \
    else _fail(label, string("Expected: true  Got: false\n           Location: ") + __FILE__ + ":" + to_string(__LINE__)); \
} catch(const exception& _e){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_e.what()<<"\n"; \
} catch(...){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n"; } } while(0)

// ASSERT_FALSE
#define ASSERT_FALSE(expr, label) do { try { \
    bool _r = (bool)(expr); \
    if (!_r) _pass(label); \
    else _fail(label, string("Expected: false  Got: true\n           Location: ") + __FILE__ + ":" + to_string(__LINE__)); \
} catch(const exception& _e){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_e.what()<<"\n"; \
} catch(...){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n"; } } while(0)

// ASSERT_EQ
#define ASSERT_EQ(actual, expected, label) do { try { \
    auto _a=(actual); auto _e=(expected); \
    if (_a==_e) { _pass(label); } \
    else { ostringstream _o; _o<<"Expected: "<<_e<<"\n           Got:      "<<_a \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__; _fail(label,_o.str()); } \
} catch(const exception& _e2){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_e2.what()<<"\n"; \
} catch(...){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n"; } } while(0)

// ASSERT_NEQ
#define ASSERT_NEQ(actual, expected, label) do { try { \
    auto _a=(actual); auto _e=(expected); \
    if (!(_a==_e)) { _pass(label); } \
    else { ostringstream _o; _o<<"Expected: NOT "<<_e<<"\n           Got: "<<_a \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__; _fail(label,_o.str()); } \
} catch(const exception& _e2){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_e2.what()<<"\n"; \
} catch(...){ TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n"; } } while(0)

// ASSERT_DOUBLE_EQ
#define ASSERT_DOUBLE_EQ(actual, expected, label) do { try { \
    double _a=(double)(actual),_e=(double)(expected); \
    if((_a-_e)<0.001&&(_e-_a)<0.001){_pass(label);} \
    else{ostringstream _o;_o<<"Expected: "<<_e<<"  Got: "<<_a \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__;_fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// ASSERT_DOUBLE_GT
#define ASSERT_DOUBLE_GT(actual, thr, label) do { try { \
    double _a=(double)(actual),_t=(double)(thr); \
    if(_a>_t){_pass(label);} \
    else{ostringstream _o;_o<<"Expected: >"<<_t<<"  Got: "<<_a \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__;_fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// ASSERT_DOUBLE_GTE
#define ASSERT_DOUBLE_GTE(actual, thr, label) do { try { \
    double _a=(double)(actual),_t=(double)(thr); \
    if(_a>=_t){_pass(label);} \
    else{ostringstream _o;_o<<"Expected: >="<<_t<<"  Got: "<<_a \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__;_fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// ASSERT_VEC_SIZE
#define ASSERT_VEC_SIZE(vec, sz, label) do { try { \
    int _g=(int)(vec).size(),_w=(int)(sz); \
    if(_g==_w){_pass(label);} \
    else{ostringstream _o;_o<<"Expected size: "<<_w<<"  Got: "<<_g \
        <<"\n           Location: "<<__FILE__<<":"<<__LINE__;_fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

#define ASSERT_VEC_EMPTY(vec, label) ASSERT_VEC_SIZE(vec, 0, label)

// ASSERT_VEC_CONTAINS
#define ASSERT_VEC_CONTAINS(vec, val, label) do { try { \
    vector<string> _v=(vec); string _w=string(val); \
    bool _f=false; for(auto& _x:_v) if(_x==_w){_f=true;break;} \
    if(_f){_pass(label);} \
    else{ostringstream _o;_o<<"Expected to contain: \""<<_w<<"\"\n           Vector: ["; \
        for(auto& _x:_v) _o<<"\""<<_x<<"\" "; _o<<"]\n           Location: "<<__FILE__<<":"<<__LINE__; \
        _fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// ASSERT_VEC_NOT_CONTAINS
#define ASSERT_VEC_NOT_CONTAINS(vec, val, label) do { try { \
    vector<string> _v=(vec); string _w=string(val); \
    bool _f=false; for(auto& _x:_v) if(_x==_w){_f=true;break;} \
    if(!_f){_pass(label);} \
    else{ostringstream _o;_o<<"Expected NOT to contain: \""<<_w<<"\"\n           Vector: ["; \
        for(auto& _x:_v) _o<<"\""<<_x<<"\" "; _o<<"]\n           Location: "<<__FILE__<<":"<<__LINE__; \
        _fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// ASSERT_PAIR_EQ
#define ASSERT_PAIR_EQ(actual, fst, snd, label) do { try { \
    auto _p=(actual); string _f=string(fst),_s=string(snd); \
    if(_p.first==_f&&_p.second==_s){_pass(label);} \
    else{ostringstream _o;_o<<"Expected: {\""<<_f<<"\", \""<<_s<<"\"}\n           Got:      {\""<<_p.first<<"\", \""<<_p.second<<"\"}\n           Location: "<<__FILE__<<":"<<__LINE__; \
        _fail(label,_o.str());} \
} catch(const exception& _ex){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- "<<_ex.what()<<"\n"; \
} catch(...){TESTS_RUN++;TESTS_FAILED++;TESTS_CRASH++; \
    cout<<"    [CRASH] "<<string(label)<<" -- unknown exception\n";} } while(0)

// SAFE_IDX
#define SAFE_IDX(vec, idx, fallback) \
    ((int)(vec).size() > (int)(idx) ? (vec)[(idx)] : (fallback))

// installSignalHandlers -- no-op in this version (try/catch only)
static void installSignalHandlers() {}

static void printSummary() {
    cout << "\n============================================================\n";
    cout << "  FINAL RESULTS\n";
    cout << "============================================================\n";
    cout << "  Total:   " << TESTS_RUN    << "\n";
    cout << "  Passed:  " << TESTS_PASSED << "\n";
    cout << "  Failed:  " << (TESTS_FAILED-TESTS_CRASH) << "  (wrong answer)\n";
    cout << "  Crashed: " << TESTS_CRASH  << "  (exception thrown)\n";
    if (TESTS_FAILED==0)
        cout << "  Status:  ALL TESTS PASSED\n";
    else
        cout << "  Status:  " << TESTS_FAILED << " failure(s)\n";
    cout << "============================================================\n";
}
