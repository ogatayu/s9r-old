/**
 * @file fifo.h
 */
#pragma once

class FIFO {
private:
    void         *buffer_;    /*!< FIFOのデータ格納先 */
    int           blockSize_; /*!< buffer_ の読み書き単位サイズ[byte] */
    int           capacity_;  /*!< FIFOに格納できるデータの数 */
    int           length_;    /*!< 現在FIFOに格納されているデータの数 */
    unsigned long tail_;      /*!< 次にデータを格納する場所へのオフセット */
    unsigned long head_;

public:
    FIFO( int capacity, int block_size );
    ~FIFO( void );

    void Put( void *data );
    void Get( void *data );
    void SnoopFromTail( void *data, int num );
};
