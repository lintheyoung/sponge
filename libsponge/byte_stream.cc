#include "byte_stream.hh"
#include <algorithm>
#include <sstream>
#include <iostream>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
// 应该是一个虚拟代码
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// 构造函数, 初始化容量
ByteStream::ByteStream(const size_t capacity){
    this->_capacity = capacity;
}

// 写方法, 往字节流中写入尽可能多的数据, 返回写入成功数据的字节数
// 要是输入的data长度过长, 就只保证小于_capacity的能被写入
size_t ByteStream::write(const string &data) {
    // 开始写入字节流, 在头部插入元素
    // 要是此时的_cur_used已经满了, 就只能写入一定的量
    // 获得输入的数据的长度
    size_t input_len = data.size();

    // 判断能够写入多少, 就只能写入最小的一个
    size_t write_able_size = min((this->_capacity - this->_cur_used), input_len);
    // 然后全部写入
    for(size_t i = 0; i < write_able_size; ++i){
        // 按照顺序一个一个写入
        // 从头部一个一个塞进去
        this->_deque.emplace_front(data[i]);
        this->_cur_used++;
        this->_write_cnt++;
    }
    return write_able_size;
}

//! \param[in] len bytes will be copied from the output side of the buffer
// 把其中结点给输出
string ByteStream::peek_output(const size_t len) const {
    // 输出一定长度的deque内容
    stringstream buf;
    // deque支持直接按照索引查找
    // 其实使用队列并不是很必要, 使用一个数组就好了
    for(size_t i = 0, j = this->_cur_used; i < len && j >= 1; i++, j--){
        // 从尾部一个一个读取
        buf << this->_deque[j - 1];
    }

    return buf.str();
}

//! \param[in] len bytes will be removed from the output side of the buffer
// 直接把头结点给pop出
void ByteStream::pop_output(const size_t len){
    // 直接去除相关的元素
    auto cur_used_tmp = this->_cur_used;
    for(size_t i = 0, j = cur_used_tmp; i < len && j >= 1; i++, j--){
        // deque自己已经给resize过了, 在pop的时候, 是把之前的里面就有的元素(size决定), 给一个一个的pop出来
        this->_deque.pop_back();
        // 并且要更新目前的使用情况
        this->_cur_used--;
        this->_read_cnt++;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
// 读取其中长度为len长度的字节
// 只读, 读取一定长度的输入数据
// 在读取之后, 就要将其置为空(pop掉)
std::string ByteStream::read(const size_t len) {
    auto str = peek_output(len);
    pop_output(len);
    return str;
}

// TODO 这个不懂啥意思
// 并且没有人调用它诶
void ByteStream::end_input(){
    // 判断是否已经
    this->_is_end = true;
}

// 判断是否结束输入, 返回is_end即可
bool ByteStream::input_ended() const{
    return this->_is_end;
}

// 查看当前可从流中读取的最大数量, 返回已用的大小
size_t ByteStream::buffer_size() const{
    return this->_cur_used;
}

// 判断是否为空函数, 可以通过已用大小是否为0来实现
bool ByteStream::buffer_empty() const{
    return this->_cur_used == 0;
}

// 判断输出是否已经达到结尾, 内存容量为空且输入结束, 则输出已经达到结尾
bool ByteStream::eof() const{
    return buffer_empty() && input_ended();
}

// 查看已写
size_t ByteStream::bytes_written() const{
    return this->_write_cnt;
}

// 已读函数
size_t ByteStream::bytes_read() const{
    return this->_read_cnt;
}

size_t ByteStream::remaining_capacity() const{
    return this->_capacity - this->_cur_used;
}