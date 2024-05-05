#include <vector>
#include <poll.h>

using std::vector;

class pollfd_container {
private:
    vector<pollfd> fds;
public:
    void add(int fd, short events);
    void erase(int index);
    void eraseall();

    pollfd get_pollfd(int index);
    pollfd* get_pointer();
    nfds_t get_nfds();
};