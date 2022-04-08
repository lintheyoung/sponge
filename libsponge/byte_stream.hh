#ifndef SPONGE_LIBSPONGE_BYTE_STREAM_HH
#define SPONGE_LIBSPONGE_BYTE_STREAM_HH

#include <string>
#include <deque>

//! \brief An in-order byte stream.

//! Bytes are written on the "input" side and read from the "output"
//! side.  The byte stream is finite: the writer can end the input,
//! and then no more bytes can be written.
class ByteStream {
  private:
    size_t _capacity = 0; // 容量的大小
    size_t _cur_used = 0; // 目前所使用的流量
    // TODO deque和queue的区别是什么呢?
    // 一定要采用这种{}的初始化方式
    std::deque<char> _deque{}; 

    // 就是还要统计历史上一共写入了多少和读出了多少
    size_t _read_cnt = 0;
    size_t _write_cnt = 0; 
    bool _is_end = false;


    bool _error{};  //!< Flag indicating that the stream suffered an error.

  public:
    //! Construct a stream with room for `capacity` bytes.
    // 构造函数, 构造方法中里面有capacity表示有限字节流的容量, 因此我们需要一个参数表示容量的大小
    // 因为是相对于是class private中一个参数, 所有使用capacity_表示
    ByteStream(const size_t capacity);

    //! \name "Input" interface for the writer
    //!@{

    //! Write a string of bytes into the stream. Write as many
    //! as will fit, and return how many were written.
    //! \returns the number of bytes accepted into the stream
    // 写方法, 往字节流中写入尽可能多的数据, 返回写入成功数据的字节数
    size_t write(const std::string &data);

    //! \returns the number of additional bytes that the stream has space for
    // 查看剩余容量的方法, 采用容量-已有数据长度的即可实现
    size_t remaining_capacity() const;

    //! Signal that the byte stream has reached its ending
    // 表示输入字节流已经到达末尾, 我们需要一个bool遍历is_end来判断是否结束输入, false表示未结束输入,
    // true表示结束输入
    void end_input();

    //! Indicate that the stream suffered an error.
    void set_error() { _error = true; }
    //!@}

    //! \name "Output" interface for the reader
    //!@{

    //! Peek at next "len" bytes of the stream
    //! \returns a string
    // 查看下次字节流中的len长度的字节, 可以利用一个字符串截取从0到len的子字符并返回, 要注意下越界问题
    std::string peek_output(const size_t len) const;

    //! Remove bytes from the buffer
    // 从字节流中移除len长度的字节, 使用字符串删除函数, 对0到len的字符串进行删除, 还是要注意越界问题
    void pop_output(const size_t len);

    //! Read (i.e., copy and then pop) the next "len" bytes of the stream
    //! \returns a string
    // 读取len长度的字节, 可以调用前面已经实现的查看与删除函数来实现本函数
    std::string read(const size_t len);

    //! \returns `true` if the stream input has ended
    // 判断是否结束输入, 返回is_end即可
    bool input_ended() const;

    //! \returns `true` if the stream has suffered an error
    bool error() const { return _error; }

    //! \returns the maximum amount that can currently be read from the stream
    // 查看当前可从流中读取的最大数量, 返回已使用大小即可
    size_t buffer_size() const;

    //! \returns `true` if the buffer is empty
    // 判断是否为空函数, 可通过已用大小是否为0来实现
    bool buffer_empty() const;

    //! \returns `true` if the output has reached the ending
    // 判断输出是否已经达到结尾, 内存中容量为空且输入结束, 则输出已达到结尾
    bool eof() const;
    //!@}

    //! \name General accounting
    //!@{

    //! Total number of bytes written
    // 查看已读, 已读函数, 我们需要额外的两个变量, write_cnt, read_cnt来储存这两个值
    size_t bytes_written() const;

    //! Total number of bytes popped
    size_t bytes_read() const;
    //!@}
};

#endif  // SPONGE_LIBSPONGE_BYTE_STREAM_HH
