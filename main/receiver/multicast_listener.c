#include "multicast_listener.h"

#include "esp_wifi.h"
#include "esp_log.h"

#include "lwip/sockets.h"

static const char *TAG = "multicast";

static int esp_join_multicast_group(int sockfd)
{
    struct ip_mreq imreq;
    struct in_addr iaddr;
    int err = 0;

    // configure interface for sending multicast messages
    esp_netif_ip_info_t ip_info = { 0 };
    err = esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get IP address info. Error 0x%x", err);
        goto err;
    }

    inet_addr_from_ip4addr(&iaddr, &ip_info.ip);
    err = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(struct in_addr));
    if (err < 0)
    {
        ESP_LOGE(TAG, "Failed to set IP_MULTICAST_IF. Error %d", errno);
        goto err;
    }

    // configure multicast group address to listen on
    inet_aton(MULTICAST_GROUP_IP, &imreq.imr_multiaddr.s_addr);

    err = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                     &imreq, sizeof(struct ip_mreq));
    if (err < 0)
    {
        ESP_LOGE(TAG, "Failed to set IP_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }

err:
    return err;
}

esp_err_t esp_recv_multicast(socket_command_callback_t command_callback)
{
    esp_err_t err = ESP_FAIL;
    struct sockaddr_in saddr = {0};
    struct sockaddr_in from_addr = {0};
    socklen_t from_addr_len = sizeof(struct sockaddr_in);

    // should be aligned with esp_send_multicast payload
    int udp_server_buf[4] = {0};

    // create an IPv4 UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        ESP_LOGE(TAG, "Create UDP socket fail");
        return err;
    }

    // binding sockets
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(CONFIG_MULTICAST_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret != ESP_OK)
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

    // join multicast group
    ret = esp_join_multicast_group(sockfd);
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to join multicast group");
        goto exit;
    }

    // call the recvfrom interface to receive multicast data
    while (1)
    {
        ret = recvfrom(sockfd, udp_server_buf, sizeof(udp_server_buf),
                       0, (struct sockaddr *)&from_addr, (socklen_t *)&from_addr_len);
        if (ret > 0)
        {
            ESP_LOGI(TAG, "Receive udp multicast from %s:%d, data is %d",
                    inet_ntoa(((struct sockaddr_in *)&from_addr)->sin_addr),
                    ntohs(((struct sockaddr_in *)&from_addr)->sin_port), 
                    udp_server_buf[0]);

            command_callback(udp_server_buf, sizeof(udp_server_buf) / sizeof(udp_server_buf[0]));
        }
    }

    //free(udp_server_buf);

exit:
    close(sockfd);
    return err;
}
