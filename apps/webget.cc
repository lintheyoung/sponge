#include "util.hh"
#include "socket.hh"

#include <cstdlib>
#include <iostream>

using namespace std;

void get_URL(const string &host, const string &path) {
	TCPSocket tcpSocket;

	tcpSocket.connect(Address(host, "http")); // 创建连接
    // 实际在shell写入的command
	tcpSocket.write("GET " + path + " HTTP/1.1\r\n" + "HOST: " + host + "\r\n" + "Connection: close\r\n"); // 继承自FileDescriptor的写方法
	
    tcpSocket.shutdown(SHUT_WR); // 告诉服务器我已完成请求

	while(!tcpSocket.eof()){ // 读取服务器端返回的数据
		cout << tcpSocket.read();
	}

	tcpSocket.close();
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 0) {
            abort();  // For sticklers: don't try to access argv[0] if argc <= 0.
        }

        // The program takes two command-line arguments: the hostname and "path" part of the URL.
        // Print the usage message unless there are these two arguments (plus the program name
        // itself, so arg count = 3 in total).
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " HOST PATH\n";
            cerr << "\tExample: " << argv[0] << " stanford.edu /class/cs144\n";
            return EXIT_FAILURE;
        }

        // Get the command-line arguments.
        const string host = argv[1];
        const string path = argv[2];

        // Call the student-written function.
        get_URL(host, path);
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}