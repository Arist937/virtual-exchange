#include <unistd.h>

#include "include/containers.h"

void pollfd_container::add(int fd, short events) {
    pollfd new_fd;
    new_fd.fd = fd;
    new_fd.events = events;

    fds.push_back(new_fd);
}

void pollfd_container::erase(int index) {
    close(fds[index].fd);

    int last = fds.size() - 1;
    fds[index] = fds[last];

    fds.pop_back();
}

void pollfd_container::eraseall() {
    int nfds = fds.size();
    for (int i = 0; i < nfds; ++i) {
        close(fds[i].fd);
    }

    while (!fds.empty()) {
        fds.pop_back();
    }
}

pollfd pollfd_container::get_pollfd(int index) {
    return fds[index];
}

pollfd* pollfd_container::get_pointer() {
    return &fds[0];
}

nfds_t pollfd_container::get_nfds() {
    nfds_t nfds = fds.size();
    return nfds;
}