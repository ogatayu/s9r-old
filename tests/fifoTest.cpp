#include <gtest/gtest.h>

#include "fifo.h"

namespace {
    class FifoTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    public:
    };

    // 一回ずつ
    TEST_F(FifoTest, OnceForEach)
    {
        const int cycle_num = 8;
        int put_data = 0;
        int get_data = 0;

        FIFO* fifo = new FIFO( cycle_num, sizeof(int) );
        EXPECT_NE( nullptr, fifo );

        for(int ix=0; ix<(cycle_num*2); ix++) {
            put_data = ix;
            get_data = 0;
            fifo->Put( &put_data );
            fifo->Get( &get_data );
            EXPECT_EQ( put_data, get_data );
        }
        // 取り尽くしたのでもう取れない
        get_data = -1;
        fifo->Get( &get_data );
        EXPECT_EQ( get_data, -1 );

        delete fifo;
    }

    // 最大数分
    TEST_F(FifoTest, UpToCapacity)
    {
        const int cycle_num = 8;
        int put_data = 0;
        int get_data = 0;

        FIFO* fifo = new FIFO( cycle_num, sizeof(int) );
        EXPECT_NE( nullptr, fifo );

        // 1周(全部取れる)
        for(int ix=0; ix<cycle_num; ix++) {
            put_data = ix;
            fifo->Put( &put_data );
        }
        for(int ix=0; ix<cycle_num; ix++) {
            get_data = 0;
            fifo->Get( &get_data );
            EXPECT_EQ( ix, get_data );
        }

        // 取り尽くしたのでもう取れない
        get_data = -1;
        fifo->Get( &get_data );
        EXPECT_EQ( get_data, -1 );


        // 2周(後半の1周分しか取れない)
        for(int ix=0; ix<cycle_num*2; ix++) {
            put_data = ix;
            fifo->Put( &put_data );
        }
        for(int ix=8; ix<cycle_num*2; ix++) {
            get_data = 0;
            fifo->Get( &get_data );
            EXPECT_EQ( ix, get_data );
        }

        // 取り尽くしたのでもう取れない
        get_data = -1;
        fifo->Get( &get_data );
        EXPECT_EQ( get_data, -1 );

        delete fifo;
    }

    TEST_F(FifoTest, SnoopFromTail_1)
    {
        const int cycle_num = 8;
        int put_data = 0;
        int get_data[cycle_num];

        FIFO* fifo = new FIFO( cycle_num, sizeof(int) );
        EXPECT_NE( nullptr, fifo );

        // put test data
        for(int ix=0; ix<cycle_num; ix++) {
            put_data = ix;
            fifo->Put( &put_data );
        }

        // snoop
        memset( get_data, 0, sizeof(get_data) );
        fifo->SnoopFromTail( get_data, cycle_num );

        // assert
        for(int ix=0; ix<cycle_num; ix++) {
            EXPECT_EQ( ix, get_data[ix] );
        }

        // なくなってはいないのでGetするととれる
        for(int ix=0; ix<cycle_num; ix++) {
            get_data[0] = 0;
            fifo->Get( &get_data[0] );
            EXPECT_EQ( ix, get_data[0] );
        }

        delete fifo;
    }


    TEST_F(FifoTest, SnoopFromTail_2)
    {
        const int cycle_num = 8;
        int put_data = 0;
        int get_data[cycle_num];

        FIFO* fifo = new FIFO( cycle_num, sizeof(int) );
        EXPECT_NE( nullptr, fifo );

        // put test data(half num)
        for(int ix=0; ix<cycle_num/2; ix++) {
            put_data = ix;
            fifo->Put( &put_data );
        }

        // snoop
        memset( get_data, 0, sizeof(get_data) );
        fifo->SnoopFromTail( get_data, cycle_num );

        // 半分しか入れてないので、半分は0になる
        for(int ix=0; ix<cycle_num/2; ix++) {
            EXPECT_EQ( 0, get_data[ix] );
        }
        for(int ix=cycle_num/2; ix<cycle_num/2; ix++) {
            EXPECT_EQ( 0, get_data[ix] );
        }

        // なくなってはいないのでGetするととれる
        for(int ix=0; ix<cycle_num/2; ix++) {
            get_data[0] = 0;
            fifo->Get( &get_data[0] );
            EXPECT_EQ( ix, get_data[0] );
        }

        delete fifo;
    }
}
