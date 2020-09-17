#include <vector>
#include <fcntl.h>
#include <iostream>

#include <err.h>

#include <time.h>

#include <vector>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <sys/select.h>

#include <sys/time.h>

#include <stdio.h>

enum Direction {
  DIR_IN,
  DIR_OUT
};
//enum为枚举类型，表示该类型只有这两个值

struct ClientCtx {
  int fd;
  Direction dir;
  std::string rd_buf;
  std::string wr_buf;
};

bool running = true;
std::vector<ClientCtx> clients;

int listen_on(int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
    //这个函数建立一个协议族为domain、协议类型为type、协议编号为protocol的套接字文件描述符。如果函数调用成功，会返回一个标识这个套接字的文件描述符，失败的时候返回-1。
  if (fd < 0) {
    err(1, "socket");
      //将第二个形参输出到stderr并调用exit()
  }
  int flag = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) != 0) {
      //设置与某个套接字关联的选项，成功会返回0
    err(1, "setsockopt");
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET; //地址族
  addr.sin_addr.s_addr = INADDR_ANY;  //32位IPV4地址
  addr.sin_port = htons(port);  //16位TCP/UDP端口号
  if (bind(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0) {
      //把一个本地协议地址赋予一个套接字，第二个参数是一个指向特定协议的地址结构的指针，第三个参数是该地址结构的长度。
    err(1, "bind");
  }
  if (listen(fd, 100) != 0) {
      //listen函数把一个未连接的套接字转换成一个被动套接字，指示内核应该接受指向该套接字的连接请求。第二个参数规定了内核应为相应套接字排队的最大连接个数。
    err(1, "listen");
  }
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0) {
      //fcntl系统调用可以用来对已打开的文件描述符进行各种控制操作以改变已打开文件的的各种属性，F_SETFL是设置文件打开方式位第三个参数指定方式，即非阻塞
    err(1, "fcntl");
  }
  return fd;
}

void handle_new_connections(int listen_fd) {
  for (;;) {
    int fd = accept(listen_fd, nullptr, nullptr);
      //在一个套接口接受一个连接，在错误时返回-1，并设置全局错误变量 errno。
    if (fd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      err(1, "accept");
    }
    if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0) {
      err(1, "fcntl");
    }
    clients.push_back({
        //函数将一个新的元素加到vector的最后面，位置为当前最后一个元素的下一个元素
      .fd = fd,
      .dir = DIR_IN,
    });
    clients.back().wr_buf += "Hello, [" + std::to_string(fd) + "]\n";
      //back() 函数返回当前vector最末一个元素的引用
      //to_string()将数值转化为字符串。返回对应的字符串。
  }
}

bool handle_read(ClientCtx& client) {
  char buf[128];
  for (;;) {
    int r = read(client.fd, buf, sizeof(buf));
    if (r < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      err(1, "read %d", client.fd);
    }
    if (r == 0) {
      return false;
    }
    if (r > 0) {
      client.rd_buf.append(buf, r);
        //append函数是向string的后面追加字符或字符串
      auto eol = client.rd_buf.find('\n');
        //返回值是字母在母串中的位置(下标记录),如果没有找到,那么会返回一个特别的标记npos
      if (eol != std::string::npos) {
        if (client.rd_buf.substr(0, eol).find("exit") != std::string::npos) {
            //substr是返回一个str，是从第一个参数开始n个字符的拷贝
          running = false;
        }
        client.wr_buf += client.rd_buf.substr(0, eol+1);
        client.rd_buf = client.rd_buf.substr(eol+1);
        client.dir = DIR_OUT;
      }
    }
  }
  return true;
}

bool handle_write(ClientCtx& client) {
  for (;;) {
    if (client.wr_buf.empty()) {
        //判断容器是否为空
      client.dir = DIR_IN;
      break;
    }

    int written = write(client.fd, &client.wr_buf[0], client.wr_buf.size());
    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      err(1, "write");
    }
    if (written == 0) {
      return false;
    }
    if (written > 0) {
      client.wr_buf = client.wr_buf.substr(written);
    }
  }
  return true;
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
    //刷新流
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
    //定义流 stream 应如何缓冲，这里_IONBF是无缓冲

  // main listening socket
  int listen_fd = listen_on(21337);
    //设定非阻塞监听该端口
  std::cout << "Listening on 21337" << std::endl;

  fd_set readset;
  fd_set writeset;

  while (running) {
      //全局变量，true

    FD_ZERO(&writeset);
    FD_ZERO(&readset);
      //将指定的文件描述符集清空，在对文件描述符集合进行设置前，必须对其进行初始化，如果不清空，由于在系统分配内存空间后，通常并不作清空处理，所以结果是不可知的。

    FD_SET(listen_fd, &readset);
      //用于在文件描述符集合中增加一个新的文件描述符。
    int max_fd = listen_fd;

    for (const ClientCtx& client : clients) {
        //将clients里的东西一个个给client进行for循环
      if (client.dir == DIR_OUT) {
        FD_SET(client.fd, &writeset);
      } else {
        FD_SET(client.fd, &readset);
      }
      max_fd = std::max(max_fd, client.fd);
    }

    int ret = select(max_fd + 1, &readset, &writeset, nullptr, nullptr);
      //select()用来等待文件描述词状态的改变。参数n代表最大的文件描述词加1，参数readfds、writefds和exceptfds 称为描述词组，是用来回传该描述词的读，写或例外的状况。最后一个参数为timeout
    if (ret > 0) {
      if (FD_ISSET(listen_fd, &readset)) {
          //用来测试描述词组set中相关fd的位是否为真
        handle_new_connections(listen_fd);
      }

      for (auto it = clients.begin(), end = clients.end(); it != end; ++it) {
          //auto用来简化变量初始化
        ClientCtx& client = *it;
        const int fd = client.fd;

        if (FD_ISSET(fd, &readset)) {
          if (!handle_read(client)) {
            close(fd);
            it = clients.erase(it);
              //删除pos处的一个字符
            continue;
          }
        } else if (FD_ISSET(fd, &writeset)) {
          if (!handle_write(client)) {
            close(fd);
            it = clients.erase(it);
            continue;
          }
        }
      }

    } else if (ret < 0 && errno != EINTR) {
      err(1, "select");
    }
  }
}
