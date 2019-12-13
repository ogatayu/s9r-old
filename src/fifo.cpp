/**
 * @file    fifo.cpp
 * @brief   Implement of FIFO buffer
 *
 * @author  Yuya Ogata
 */

#include <stdlib.h>


class FIFO {
    private:
        void         *buffer_;    /*!< FIFOのデータ格納先 */
        size_t        blockSize_; /*!< buffer_ の読み書き単位サイズ[byte] */
        size_t        capacity_;  /*!< FIFOに格納できるデータの数 */
        size_t        length_;    /*!< 現在FIFOに格納されているデータの数 */
        unsigned long tail_;      /*!< 次にデータを格納する場所へのオフセット */
        unsigned long head_;

    public:
        FIFO();
        ~FIFO( void );

        void put(int item);
        int  get( void );
};


/**
 * @brief FIFO
 *
 * @param[in]  total_size データ格納バッファのサイズ[byte]
 * @param[in]  block_size データ格納バッファの読み書き単位サイズ[byte]
 */
inline FIFO:FIFO( size_t capacity, size_t block_size )
{
    if( capacity & (capacity-1) ) {
        /* 2のべき乗数でなければ拒否 */
        return;
    }

    if( total_size % block_size != 0 ) {
        /* 割り切れなかったらblock_sizeが不適切とみなす */
        return;
    }

    /* FIFO情報初期化 */
    capacity_  = capacity;
    blockSize_ = block_size;
    length_    = 0;
    tail_      = 0;
    head_      = 0;
    buffer_    = malloc( capacity * block_size );;
}

/**
 * @brief ~FIFO
 */
inline FIFO:~FIFO( void )
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
inline int FIFO:put( void *data )
{
    /* 引数チェック */
    if( data_ == NULL ) {
        return (-1);
    }
    if( length_ >= capacity_ ) {
        return (-1);
    }

    /* FIFOへデータ格納 */
    memcpy( (unsigned char *)data_ + (tail_ * blockSize_), data, blockSize_ );

    /* FIFO情報更新 */
    tail_ = (tail_+1) & (capacity_-1);
    length_++;

    return 0;
}

/**
 * @brief FIFOからデータ取得
 *
 * @param[out] data 読み込むデータを格納するバッファへのポインタ
 * @retval  0 成功
 * @retval -1 失敗
 */
inline int FIFO:get( void *data )
{
    /* 引数チェック */
    if( data_ == NULL ) {
        return (-1);
    }
    if( length_ <= 0 ) {
        return (-1);
    }

    /* FIFOからデータ取得 */
    memcpy( data, (unsigned char *)data_ + (head_ * blockSize_), blockSize_ );

    /* FIFO情報更新 */
    head_ = (head_ + 1) % capacity_;
    length_--;

    return 0;
}
