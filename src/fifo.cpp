/**
 * @file    fifo.cpp
 * @brief   Implement of FIFO buffer
 *
 * @author  Yuya Ogata
 */

#include <stdlib.h>
#include <string.h>

#include "fifo.h"


/**
 * @brief FIFO
 *
 * @param[in]  total_size データ格納バッファのサイズ[byte]
 * @param[in]  block_size データ格納バッファの読み書き単位サイズ[byte]
 */
FIFO::FIFO( int capacity, int block_size )
{
    if( capacity & (capacity-1) ) {
        /* 2のべき乗数でなければ拒否 */
        return;
    }

    if( capacity % block_size != 0 ) {
        /* 割り切れなかったらblock_sizeが不適切とみなす */
        return;
    }

    /* FIFO情報初期化 */
    capacity_  = capacity;
    blockSize_ = block_size;
    length_    = 0;
    tail_      = 0;
    head_      = 0;
    buffer_    = malloc( capacity * block_size );
    memset( buffer_, 0, capacity * block_size);
}

/**
 * @brief ~FIFO
 */
FIFO::~FIFO( void )
{
    free( buffer_ );
}

/**
 * @brief FIFOへデータ追加
 *
 * @param[in] data 追加するデータ
 * @retval  0 成功
 * @retval -1 失敗
 */
void FIFO::Put( void *data )
{
#if 0
    /* 引数チェック */
    if( data == NULL ) {
        return (-1);
    }
    if( length_ >= capacity_ ) {
        return (-1);
    }
#endif

    /* FIFOへデータ格納 */
    memcpy( (unsigned char *)buffer_ + (tail_ * blockSize_), data, blockSize_ );

    /* FIFO情報更新 */
    tail_ = (tail_+1) & (capacity_-1);
    length_++;

    return;
}

/**
 * @brief FIFOからデータ取得
 *
 * @param[out] data 読み込むデータを格納するバッファへのポインタ
 * @retval  0 成功
 * @retval -1 失敗
 */
int FIFO::Get( void *data )
{
#if 0
    /* 引数チェック */
    if( data == NULL ) {
        return (-1);
    }
    if( length_ <= 0 ) {
        return (-1);
    }
#endif

    /* FIFOからデータ取得 */
    memcpy( data, (unsigned char *)buffer_ + (head_ * blockSize_), blockSize_ );

    /* FIFO情報更新 */
    head_ = (head_ + 1) % capacity_;
    length_--;

    return 0;
}

/**
 * @brief 古いデータからすべてとる
 *
 * @param[out] data 読み込むデータを格納するバッファへのポインタ
 * @retval  0 成功
 * @retval -1 失敗
 */
void FIFO::SnoopFromTail( void *data, int num )
{
    // round
    if( capacity_ < (tail_ * num) ) {
        int first_num  = capacity_ - tail_;
        int latter_num = num - first_num;
        memcpy(
            data,
            (unsigned char *)buffer_ + (tail_ * blockSize_),
            first_num * blockSize_
            );
        memcpy(
            (unsigned char *)data + (first_num * blockSize_),
            (unsigned char *)buffer_,
            latter_num * blockSize_
            );
    }
    else {
        memcpy(
            data,
            (unsigned char *)buffer_ + (tail_ * blockSize_),
            blockSize_ * num
            );
    }
}
