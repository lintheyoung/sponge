#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// 构造函数, 限制了接收和处理的容量大小
// 不需要在修改
// 暂时不是很明白要修改什么
// TODO 不是很懂要这么写
StreamReassembler::StreamReassembler(const size_t capacity): _output(capacity), _capacity(capacity), _unacceptable(capacity){}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
// 接收子串，并将新的连续的字节写入流中。
// 只写入新的，如果有重合部分则丢弃重合部分。
// 这里也要注意容量，超出容量部分要丢弃。
// 这一个应该是核心代码, 就是按照编号把所有的数据都给合并起来.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // 用一个结构体储存所有的数据, data, index, 和eof;
    // 利用一个unordered_map实现判断index已经排好好, 要是排好了就直接给合并一起
    // 阅读先理解了项目的需求之后, 再去写出来所有的代码
    // TODO 这个其实不是很理解是为什么
    this->_hadread = this->_output.bytes_read(); // 查看有多少是已经读过的了
    this->_unacceptable = this->_hadread + this->_capacity; // 能够被处理的界限, 更新unacceptable的index

    // 变成一个数据结构
    StreamNode new_s_n = {index, data.length(), data};
    // 然后把tmp塞入到树中
    // TODO 需要分情况给塞入 
    _AddNewStreamNode(new_s_n, eof); // 把需要处理的给塞进去
    _StitchOutput(); 
    
    // 判断是否为末尾
    if(empty() && _eof){
        _output.end_input();
    }
}

// 后面把所有的函数给合并一下, 要尽可能的优雅一些
void StreamReassembler:: _AddNewStreamNode(StreamNode &new_s_n, const bool eof){
    // 检查容量限制, 如果还没有到达限制容量, 就return;
    // 去除那些超出容量限制的元素;
    // 记录这个EOF
    // 把新加入的StreamNode中的已经在_OUTPUT的, 要全部去除掉
    // 那些要处理掉的, 要进入_HandleOverlap()
    // 更新 _EOF

    // check capacity limit, if unmeet limit, return
    // cut the bytes in NEW_SEG that will overflow the _CAPACITY
    // note that the EOF should also be cut
    // cut the bytes in NEW_SEG that are already in _OUTPUT
    // _HANDLE_OVERLAP()
    // update _EOF
    
    // 已经超过容量
    // 要是已经超过了unacceptable, 就说明已经无法再处理了, 直接返回
    if(new_s_n.index >= _unacceptable){
        return;
    }

    // 判断是否已经到末尾, 用于判断是否已经处理完
    bool eof_of_this_s_n = eof;

    // 超出的字节数量
    // _unacceptable 是能够接受的最大数
    // 主要是用于计算目前所拥有的容量, 能够处理多少, 剩下还有多少没能被处理
    int overflow_bytes = new_s_n.index + new_s_n.length - _unacceptable; // 没能被处理的数量

    // 要是没成被处理的数量>0
    if(overflow_bytes > 0){
        // 能够塞入的数量
        // 这些就是能被处理的数量
        int new_length = new_s_n.length - overflow_bytes;
        // 这个应该不可能发生, 不过能更加稳健
        if(new_length <= 0){
            return;
        }
        // 并且此时就是没能处理到末尾
        eof_of_this_s_n = false;
        // 输入的长度修改
        // 就是能处理多少
        new_s_n.length = new_length;
        // 切割字符串出来
        // 修正原来输入的大小
        new_s_n.content = new_s_n.content.substr(0, new_s_n.length); 
    }

    // 要是index小于assemble时
    // 开始处理
    // 这部分代码和之前有重复的地方
    if(new_s_n.index < _unassemble){
        // 能够被unassemble处理的长度
        int new_length = new_s_n.length + new_s_n.index - _unassemble;
        if(new_length <= 0){
            return;
        }

        // 修改原来的new_s_n, 不能处理的直接舍弃
        new_s_n.length = new_length;
        // 把可以被处理的给裁切出来
        new_s_n.content = new_s_n.content.substr(_unassemble - new_s_n.index, new_s_n.length);
        new_s_n.index = _unassemble;
    }

    // 在修正之后, 到HandleOverlap中处理
    _HandleOverlap(new_s_n);

    // 判断是否已经处理到末尾
    // if EOF was reveived before, it should remain valid;
    _eof = _eof || eof_of_this_s_n;
}

// 处理超出的部分
// 先保证所有数据都被裁剪到可以被处理
void StreamReassembler:: _HandleOverlap(StreamNode &new_s_n){
    // 遍历在set中的所有
    for(auto it = _stream_node_set.begin(); it != _stream_node_set.end();){
        // 获得下一个
        // TODO 这个要debug看一下
        auto next_it = ++it;
        --it;
        // new_s_n是输入的, it是遍历的, 下面是要保证是相互包含的结果
        // 遍历所有的元素, 要是有和原来有重叠的, 就更新重叠部分
        if((new_s_n.index >= it->index && new_s_n.index < it->index + it->length) 
        || (it->index >= new_s_n.index && it->index < new_s_n.index + new_s_n.length)){
            // 要是出现相互包含的结果, 就直接采用合并的方式, 
            _MergeStreamNode(new_s_n, *it);
            // 并把原来的给删除掉
            // 只要有的都要合并
            _stream_node_set.erase(it);
        }
        it = next_it;
    }
    // 然后在把合并后的结果再次塞入到set中
    _stream_node_set.insert(new_s_n);
}

// 合并输出
void StreamReassembler:: _StitchOutput(){
    // _FIRST_UNASSEMBLED is the expected next index_FIRST_UNASSEMBLED
    // compare _STORED_SEGS.begin()->index with
    // if equals, then _STITCH_ONE_SEG() and erase this seg from set
    // continue compare until not equal or empty
    while(!_stream_node_set.empty() && _stream_node_set.begin()->index == _unassemble){
        _StitchOneStreamNode(*_stream_node_set.begin());
        _stream_node_set.erase(_stream_node_set.begin());
    }
}

void StreamReassembler:: _StitchOneStreamNode(const StreamNode &new_s_n){
    // write string of NEW_SEG into _OUTPUT
    // update _FIRST_UNASSEMBLED
    // 把处理好的new_s_n压入read文件
    // 
    _output.write(new_s_n.content);
    // 然后把_unassemble向前推进
    _unassemble += new_s_n.length;
    // both way of updating _FIRST_UNASSEMBLED is ok
    // _first_unassembled = _output.bytes_written();
}

// 合并两个字符串
void StreamReassembler::_MergeStreamNode(StreamNode &new_s_n, const StreamNode &other){
    // 新输入的index起始地址
    size_t n_index = new_s_n.index;
    // 新输入的元素的终止地址
    size_t n_end = new_s_n.index + new_s_n.length;
    // 其他输入元素起始地址
    size_t o_index = other.index;
    // 其他输入元素的终止地址
    size_t o_end = other.index + other.length;

    // 把两个给合并起来
    string new_data;
    // 例子1: other_index_start < new_index_start < new_index_end < other_index_end
    if(n_index <= o_index && n_end <= o_end){
        // 把它合并起来
        new_data = new_s_n.content + other.content.substr(n_end - o_index, n_end - o_end);
    }else if(n_index <= o_index && n_end >= o_end){
        // 在中间
        new_data = new_s_n.content;
    }else if(n_index >= o_index && n_end <= o_end){
        // 
        new_data = other.content.substr(0, n_index - o_index) + new_s_n.content + other.content.substr(n_end - o_index, n_end - o_end);
    }else{
        // if (n_index >= o_index && n_end <= o_end)
        new_data = other.content.substr(0, n_index - o_index) + new_s_n.content;
    }

    // 合并之后的最长长度
    // index要取小的
    new_s_n.index = n_index < o_index ? n_index : o_index;
    // index要取大的, 取后面那个
    new_s_n.length = (n_end > o_end ? n_end : o_end) - new_s_n.index;
    // 内容
    new_s_n.content = new_data;
}

// 已存储但未被重组的字节流数
// 统计unacceptable后元素的多少, 灰色部分
size_t StreamReassembler::unassembled_bytes() const{
    // 这一部分应该就是那些还没有被index符号要求能够合并起来的数据
    size_t unassembled_bytes = 0;
    for(auto it = _stream_node_set.begin(); it != _stream_node_set.end(); ++it){
        unassembled_bytes += it->length;
    }
    return unassembled_bytes;
}

// 重组器是否为空
bool StreamReassembler::empty() const{
    return unassembled_bytes() == 0;
}
