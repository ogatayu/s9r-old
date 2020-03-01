/**
 * @file fifo.h
 */
#pragma once

class FIFO {
private:
public:
    FIFO( size_t capacity, size_t block_size );
    ~FIFO( void );

    void         *buffer_;    /*!< FIFOのデータ格納先 */
    size_t        blockSize_; /*!< buffer_ の読み書き単位サイズ[byte] */
    size_t        capacity_;  /*!< FIFOに格納できるデータの数 */
    size_t        length_;    /*!< 現在FIFOに格納されているデータの数 */
    unsigned long tail_;      /*!< 次にデータを格納する場所へのオフセット */
    unsigned long head_;

    void Put( void *data );
    int  Get( void *data );
};
