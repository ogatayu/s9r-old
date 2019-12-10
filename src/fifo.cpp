
/**
 * @file fifo.cpp
 */


class FIFO {
    private:
        int  wp_;
        int  rp_;
        int  size_;
        int *buf_;
    public:
        FIFO();
        ~FIFO( void )
        void put(int item);
        int  get( void );
};

/**
 * @brief FIFO
 */
inline FIFO:FIFO( int size )
{
    rp_   = 0;
    wp_   = 0;
    size_ = size;
    buf_  = malloc( sizeof(int) * size );
}

/**
 * @brief ~FIFO
 */
inline FIFO:~FIFO( void )
{
    free( buf_ );
}

/**
 * @brief put
 */
inline void FIFO:put( int item )
{
    if( wp_ < rp_ ) {
    }

    wp_++;
}

/**
 * @brief get
 */
inline int FIFO:get( void )
{
    if( wp_ == rp_ ) {
        return 0;
    }
}

#endif /* __COMMON_H__ */
