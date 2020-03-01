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
FIFO::FIFO( size_t capacity, size_t block_size )
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
