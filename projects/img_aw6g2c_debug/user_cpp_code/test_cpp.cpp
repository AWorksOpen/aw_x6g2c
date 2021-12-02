/*
 * test_C++.cpp
 *
 *  Created on: 2015年3月24日
 *      Author: yangmengfei
 */
#include "apollo.h"

#include "aw_delay.h"
#include "aw_system.h"
#include "aw_task.h"
#include "aw_led.h"                     /* LED服务 */
#include "aw_buzzer.h"                  /* 蜂鸣器服务 */
#include "aw_vdebug.h"
#include "test_cpp.h"
#include <string>
#include <stdio.h>
#include <unity.h>
#include <vector>
#include <typeinfo>

using namespace std;

aw_local aw_tick_t __g_tick1 = 0,__g_tick2 = 0;
/** \brief 用来测试全局构造函数的类 */
class ctest_global_construct
{
public:
    ctest_global_construct()
    {
        __g_tick1 = aw_sys_tick_get();
        aw_task_delay(100);
        __g_tick2 = aw_sys_tick_get();
    };
    ~ctest_global_construct() {};

};

ctest_global_construct __g_test_global_construct;
aw_local void test_global_construct()
{
    aw_kprintf("__g_tick1=%d,__g_tick2=%d\n",__g_tick1,__g_tick2);
    TEST_ASSERT( (__g_tick2-__g_tick1)>=99 && (__g_tick2-__g_tick1)<=120 );
}

/** \brief 局部静态构造函数是否已经执行 */
aw_local int    __g_local_static_construct_exed = 0;
/** \brief 用来测试全局构造函数的类 */
class ctest_local_static_construct
{
public:
    ctest_local_static_construct() {__g_local_static_construct_exed = 1;};
    ~ctest_local_static_construct() {};

};

aw_local void test_local_static_construct_helper()
{
    static ctest_local_static_construct test;
}
aw_local void test_local_static_construct()
{
    if (__g_local_static_construct_exed == 0) {
        test_local_static_construct_helper();
        TEST_ASSERT_EQUAL(1,__g_local_static_construct_exed);
        test_local_static_construct_helper();
        TEST_ASSERT_EQUAL(1,__g_local_static_construct_exed);
    }
}




template <typename T>
void swap_t_func(T *a, T *b)
{
    T         c;
    c = *a;
    *a = *b;
    *b = c;

}
/** \brief 测试模板 */
aw_local void test_template()
{
    int         i_src1,i_src2,i_dst;
    double      d_src1,d_src2,d_dst1,d_dst2;
    int         r;


    i_src1 = i_src2 = 5689;
    i_dst = 256478;

    TEST_ASSERT_NOT_EQUAL(i_src2,i_dst);
    TEST_ASSERT_EQUAL(i_src2,i_src1);

    swap_t_func(&i_src1,&i_dst);
    TEST_ASSERT_EQUAL(i_src2,i_dst);
    TEST_ASSERT_NOT_EQUAL(i_src2,i_src1);

    d_src1 = d_src2 = 3.141592654;
    d_dst1 = d_dst2 = 2.618569423;

    swap_t_func(&d_src1,&d_dst1);
    r = 0;
    if (d_dst1 == d_src2) {
        r = 1;
    }
    TEST_ASSERT_EQUAL(1,r);
    r = 0;
    if (d_src1 == d_dst2) {
        r = 1;
    }
    TEST_ASSERT_EQUAL(1,r);
}

/** \brief 测试new */
aw_local void test_new()
{
    int            *p = NULL;
    int             r = 0;

    p = new int;
    if (NULL != p) {
        r = 1;
    }
    TEST_ASSERT_EQUAL(1,r);

    delete p;

    p = new int[256];
    if (NULL != p) {
        r = 1;
    }
    TEST_ASSERT_EQUAL(1,r);
    delete p;
}

#define TEST_VECT_COUNT         4096
/** \brief 测试vector */
aw_local void  test_vector(void)
{
    vector<int>                    *p_vect = 0;
    int                             i = 0;
    int                             r = 0;
    vector<int>::const_iterator     iter;

    p_vect = new vector<int>;
    if (NULL != p_vect) {
        r = 1;
    }
    TEST_ASSERT_EQUAL(1,r);

    // 压入数据
    for (i = 1; i<=TEST_VECT_COUNT; i++ ) {
        p_vect->push_back(i);
    }
    // 测试在vector中的数据
    i = 1;
    r = 0;
    iter =  p_vect->begin();
    while (iter != p_vect->end()) {
        if( *iter != i) {
            r = 1;
        }
        iter ++;
        i ++;
    }

    TEST_ASSERT_NOT_EQUAL(1,r);
    delete p_vect;
}


static int division(int x,int y)
{
    if(y == 0)
    {
        throw (int)-AW_EDOM;

    }
    return x/y;
}
/** \brief 测试异常 */
aw_local void  test_exception(void)
{
    int                             r = 0;
    int                             t;

    try
    {
        t = division(5,3);
        t = division(5,0);
    }
    catch (int e)
    {
       r = e;
   }

    TEST_ASSERT_EQUAL(1,t);
    TEST_ASSERT_EQUAL(-AW_EDOM,r);
}

class c_test_rtti_base
{
public:
    virtual ~c_test_rtti_base() {};
    virtual void test(){};
};

class c_test_rtti:public c_test_rtti_base
{
    virtual void test()
    {
        aw_kprintf("test\r\n");
    };
};

/** \brief 运行时类型识别 */
aw_local void  test_rtti(void)
{
    int                             r = 0;
    c_test_rtti_base                c1;
    c_test_rtti                     c2;
    c_test_rtti_base                *p;

    p = &c1;
    r = 0;
    if( typeid(*p) == typeid(c_test_rtti_base) ) {
        r = 1;
    }

    TEST_ASSERT_EQUAL(1,r);
    p = &c2;
    r = 0;
    if( typeid(*p) == typeid(c_test_rtti) ) {
        r = 1;
    }

    TEST_ASSERT_EQUAL(1,r);
}

extern "C" void test_cpp(void)
{
    aw_kprintf("\n");
    UnityBegin(__FILE__);
    RUN_TEST(test_global_construct);
    RUN_TEST(test_local_static_construct);
    RUN_TEST(test_template);
    RUN_TEST(test_new);
    RUN_TEST(test_vector);
    RUN_TEST(test_exception);
    RUN_TEST(test_rtti);
    UnityEnd();
}

