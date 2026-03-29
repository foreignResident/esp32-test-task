#include <stdlib.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_random.h"

#include "lwip/sockets.h"

#include "multicast_publisher.h"

#define MSG_BUF_LEN 4

static const char *TAG = "multicast";
static int socket_fd = -1; // -1 = not initialised, >= 0 = valid fd
static struct sockaddr_in dest_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(CONFIG_MULTICAST_PORT),
};


static void generate_random_sequence(int *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        buf[i] = esp_random() % 2;
    }
}

// returns: >= 0 (socket fd) on success, -1 on failure
int init_multicast_socket() {
    if(socket_fd != -1) return socket_fd;

    struct sockaddr_in saddr = {0};
    // configure the multicast destination address and port
    inet_aton(MULTICAST_GROUP_IP, &dest_addr.sin_addr.s_addr);

    // create an ipv4 upd socket
    // >= 0 on success, -1 on error
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        ESP_LOGE(TAG, "Create UDP socket fail");
        return socket_fd;
    }

    // bind socket
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(CONFIG_MULTICAST_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 0 on success, -1 on error
    int ret = bind(socket_fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to bind socket. Error %d", errno);
        close(socket_fd);
        socket_fd = -1;
        return ret;
    }

    // set ttl to 1 to limit packes by the local network
    uint8_t ttl = 1;
    // 0 on success, -1 on error
    ret = setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        close(socket_fd);
        socket_fd = -1;
        return ret;
    }

    return socket_fd;
}

void close_multicast_socket() {
    if (socket_fd == -1) return;
    close(socket_fd);
    socket_fd = -1;
}

esp_err_t esp_send_multicast(void)
{
    esp_err_t err = ESP_FAIL;
    if(socket_fd < 0) 
    {
        ESP_LOGE(TAG, "Socket is not initialised. Error code %d", socket_fd);
        return err;
    }

    int multicast_msg_buf[MSG_BUF_LEN];
    generate_random_sequence(multicast_msg_buf, MSG_BUF_LEN);

    // send multicast data
    // >= 0 bytes sent on success, -1 on error
    int ret = sendto(socket_fd, multicast_msg_buf, sizeof(multicast_msg_buf), 0,
                    (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
    else
    {
        ESP_LOGI(TAG, "Message sent successfully");
    }

    err = ESP_OK;

    return err;
}
