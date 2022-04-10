#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

// bytestream就是有一定容量读写储存的东西
#include "byte_stream.hh"

#include <cstdint>
#include <string>
#include <set>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    // 上一个所实现的写入和读出的模型
    ByteStream _output;  //!< The reassembled in-order byte stream
    // 每一个流的大小
    size_t _capacity;     //!< The maximum number of bytes
    // 要初始化
    size_t _hadread = 0;       // 还有多少是没有被read的
    size_t _unassemble = 0;   // 还有多少是没有被合并到一起的
    size_t _unacceptable; // 还有多少是没有被accept的
    bool _eof = false;    // 查看是否已经到了末尾

    // 定义一个输入流的储存结构体
    struct StreamNode{
      /* data */
      // 储存索引, 长度, 内容
      size_t index;
      size_t length;
      std::string content;

      // 同时在把元素塞进去set时, 需要按照一定的规则, 所以采用参数符重载的方法
      bool operator<(const StreamNode t) const{
        return index < t.index; 
      }
    };

    // 定义一个set把所有碎片化的stream内容给储存下来
    std::set<StreamNode> _stream_node_set = {};
    
    // 自定义的函数
    void _AddNewStreamNode(StreamNode &new_s_n, const bool eof); // 把新数据添加
    void _HandleOverlap(StreamNode &new_s_n); // 要是超过
    void _StitchOutput(); // 不是很懂
    void _StitchOneStreamNode(const StreamNode &new_s_n);
    void _MergeStreamNode(StreamNode &new_s_n, const StreamNode &other);

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    // 重组器的构造方法。重组器有容量控制，其内的字节流（已重组好的和未重组好的）不能超过容量，超出部分需要丢弃。
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    // 接收子串，并将新的连续的字节写入流中。只写入新的，如果有重合部分则丢弃重合部分。
    // 这里也要注意容量，超出容量部分要丢弃。
    // data：子串内容
    // index：’ data '中第一个字节的索引(按顺序排列)
    // eof：data的最后一个字节是否为整个流的最后一个字节
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    // 访问重组好的字节流
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    // 已存储但未被重组的字节流数
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    // 重组器是否为空
    bool empty() const;

    // 提交新的输入
    // void _AddNewStreamNode(StreamNode &new_s_n, const bool eof) {};
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
