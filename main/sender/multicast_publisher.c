#include <stdlib.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_random.h"

#include "lwip/sockets.h"

#include "multicast_publisher.h"

#define MSG_BUF_LEN 4

static const char *TAG = "multicast";

static void generate_random_sequence(int *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        buf[i] = esp_random() % 2;
    }
}

esp_err_t esp_send_multicast(void)
{
    esp_err_t err = ESP_FAIL;
    struct sockaddr_in saddr = {0};

    // create an ipv4 upd socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        ESP_LOGE(TAG, "Create UDP socket fail");
        return err;
    }

    // bind socket
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(CONFIG_MULTICAST_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to bind socket. Error %d", errno);
        goto exit;
    }

    // set ttl to 1 to limit packes by the local network
    uint8_t ttl = 1;
    ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto exit;
    }
    // configure the multicast destination address and port
    struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(CONFIG_MULTICAST_PORT),
    };
    inet_aton(MULTICAST_GROUP_IP, &dest_addr.sin_addr.s_addr);

    int multicast_msg_buf[MSG_BUF_LEN];
    generate_random_sequence(multicast_msg_buf, MSG_BUF_LEN);

    // send multicast data
    ret = sendto(sockfd, multicast_msg_buf, sizeof(multicast_msg_buf), 0,
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

exit:
    close(sockfd);
    return err;
}