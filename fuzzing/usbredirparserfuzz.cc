/* usbredirparserfuzz.cc -- fuzzing for usbredirparser

   Copyright 2021 Michael Hanselmann

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <array>
#include <algorithm>
#include <memory>

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <limits>

#include <sys/types.h>
#include <unistd.h>

#include <fuzzer/FuzzedDataProvider.h>

#include "usbredirfilter.h"
#include "usbredirparser.h"

namespace {
struct ParserDeleter {
    void operator()(struct usbredirparser *p) {
        usbredirparser_destroy(p);
    }
};

std::unique_ptr<struct usbredirparser, ParserDeleter> parser;
std::unique_ptr<FuzzedDataProvider> fdp;

void log(const char *format, ...)
{
#if 0
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
#endif
}

void parser_log(void *priv, int level, const char *msg)
{
    log("[%d] %s\n", level, msg);
}

int parser_read(void *priv, uint8_t *data, int count)
{
    log("%s: %d bytes\n", __func__, count);

    return fdp->ConsumeData(data, count);
}

int parser_write(void *priv, uint8_t *data, int count)
{
    log("%s: %d bytes\n", __func__, count);

    // Read over complete source buffer to detect buffer overflows on write
    void *buf = malloc(count);
    memcpy(buf, data, count);
    free(buf);

    return count;
}

void parser_device_connect(void *priv,
    struct usb_redir_device_connect_header *device_connect)
{
    log("%s: speed=%d, class=%d, subclass=%d, protocol=%d, vendor=%04x,"
        " product=%04x\n",
        __func__,
        device_connect->speed,
        device_connect->device_class,
        device_connect->device_subclass,
        device_connect->device_protocol,
        device_connect->vendor_id,
        device_connect->product_id);
}

void parser_device_disconnect(void *priv)
{
    log("%s\n", __func__);
}

void parser_reset(void *priv)
{
    log("%s\n", __func__);
}

void parser_interface_info(void *priv,
    struct usb_redir_interface_info_header *info)
{
    uint32_t i;

    log("%s:", __func__);

    for (i = 0; i < info->interface_count; i++) {
        log(" [interface %d, class %d, subclass %d, protocol %d]",
            info->interface[i], info->interface_class[i],
            info->interface_subclass[i], info->interface_protocol[i]);
    }

    log("\n");
}

void parser_ep_info(void *priv,
    struct usb_redir_ep_info_header *ep_info)
{
    int i;

    log("%s:", __func__);

    for (i = 0; i < 32; i++) {
        if (ep_info->type[i] != usb_redir_type_invalid) {
            log(" [index %d, type %d, interval %d, interface %d,"
                " max-packetsize %d]",
                i, (int)ep_info->type[i], (int)ep_info->interval[i],
                (int)ep_info->interface[i], ep_info->max_packet_size[i]);
       }
    }

    log("\n");
}

void parser_set_configuration(void *priv, uint64_t id,
    struct usb_redir_set_configuration_header *set_configuration)
{
}

void parser_get_configuration(void *priv, uint64_t id)
{
}

void parser_configuration_status(void *priv, uint64_t id,
    struct usb_redir_configuration_status_header *config_status)
{
    log("%s: id=%" PRIu64 ", status=%d, configuration=%d\n",
        __func__, id, config_status->status, config_status->configuration);
}

void parser_set_alt_setting(void *priv, uint64_t id,
    struct usb_redir_set_alt_setting_header *set_alt_setting)
{
}

void parser_get_alt_setting(void *priv, uint64_t id,
    struct usb_redir_get_alt_setting_header *get_alt_setting)
{
}

void parser_alt_setting_status(void *priv, uint64_t id,
    struct usb_redir_alt_setting_status_header *alt_setting_status)
{
    log("%s: id=%" PRIu64 ", status=%d, interface=%d, alt=%d\n",
        __func__, id,
        alt_setting_status->status,
        alt_setting_status->interface,
        alt_setting_status->alt);
}

void parser_start_iso_stream(void *priv, uint64_t id,
    struct usb_redir_start_iso_stream_header *start_iso_stream)
{
}

void parser_stop_iso_stream(void *priv, uint64_t id,
    struct usb_redir_stop_iso_stream_header *stop_iso_stream)
{
}

void parser_iso_stream_status(void *priv, uint64_t id,
    struct usb_redir_iso_stream_status_header *iso_stream_status)
{
}

void parser_start_interrupt_receiving(void *priv, uint64_t id,
    struct usb_redir_start_interrupt_receiving_header *start_interrupt_receiving)
{
}

void parser_stop_interrupt_receiving(void *priv, uint64_t id,
    struct usb_redir_stop_interrupt_receiving_header *stop_interrupt_receiving)
{
}

void parser_interrupt_receiving_status(void *priv, uint64_t id,
    struct usb_redir_interrupt_receiving_status_header *interrupt_receiving_status)
{
}

void parser_alloc_bulk_streams(void *priv, uint64_t id,
    struct usb_redir_alloc_bulk_streams_header *alloc_bulk_streams)
{
}

void parser_free_bulk_streams(void *priv, uint64_t id,
    struct usb_redir_free_bulk_streams_header *free_bulk_streams)
{
}

void parser_bulk_streams_status(void *priv, uint64_t id,
    struct usb_redir_bulk_streams_status_header *bulk_streams_status)
{
}

void parser_cancel_data_packet(void *priv, uint64_t id)
{
}

void parser_filter_reject(void *priv)
{
}

void parser_filter_filter(void *priv,
    struct usbredirfilter_rule *rules, int rules_count)
{
    usbredirfilter_free(rules);
}

void dump_data(const uint8_t *data, const int len)
{
    int i;

    if (len == 0) {
        return;
    }

    log("  ");
    for (i = 0; i < len; i++) {
        log(" %02X", (unsigned int)data[i]);
    }
    log("\n");
}

void parser_control_packet(void *priv, uint64_t id,
    struct usb_redir_control_packet_header *control_packet,
    uint8_t *data, int data_len)
{
    log("%s: id=%" PRIu64 ", endpoint=%d, request=%d, requesttype=%d,"
        " status=%d, value=%d, index=%d, length=%d\n",
        __func__, id,
        control_packet->endpoint,
        control_packet->request,
        control_packet->requesttype,
        control_packet->status,
        control_packet->value,
        control_packet->index,
        control_packet->length);
    dump_data(data, data_len);
    usbredirparser_free_packet_data(parser.get(), data);
}

void parser_bulk_packet(void *priv, uint64_t id,
    struct usb_redir_bulk_packet_header *bulk_packet,
    uint8_t *data, int data_len)
{
    log("%s: id=%" PRIu64 ", endpoint=%d, status=%d, length=%d, stream_id=%d,"
        " length_high=%d\n",
        __func__, id,
        bulk_packet->endpoint,
        bulk_packet->status,
        bulk_packet->length,
        bulk_packet->stream_id,
        bulk_packet->length_high);
    dump_data(data, data_len);
    usbredirparser_free_packet_data(parser.get(), data);
}

void parser_iso_packet(void *priv, uint64_t id,
    struct usb_redir_iso_packet_header *iso_packet,
    uint8_t *data, int data_len)
{
    log("%s: id=%" PRIu64 ", endpoint=%d, status=%d, length=%d\n",
        __func__, id,
        iso_packet->endpoint,
        iso_packet->status,
        iso_packet->length);
    dump_data(data, data_len);
    usbredirparser_free_packet_data(parser.get(), data);
}

void parser_interrupt_packet(void *priv, uint64_t id,
    struct usb_redir_interrupt_packet_header *interrupt_packet,
    uint8_t *data, int data_len)
{
    log("%s: id=%" PRIu64 ", endpoint=%d, status=%d, length=%d\n",
        __func__, id,
        interrupt_packet->endpoint,
        interrupt_packet->status,
        interrupt_packet->length);
    dump_data(data, data_len);
    usbredirparser_free_packet_data(parser.get(), data);
}

void parser_buffered_bulk_packet(void *priv, uint64_t id,
    struct usb_redir_buffered_bulk_packet_header *buffered_bulk_header,
    uint8_t *data, int data_len)
{
    log("%s: stream_id=%d, length=%d, endpoint=%d, status=%d\n",
        __func__, id,
        buffered_bulk_header->stream_id,
        buffered_bulk_header->length,
        buffered_bulk_header->endpoint,
        buffered_bulk_header->status);
    dump_data(data, data_len);
    usbredirparser_free_packet_data(parser.get(), data);
}

void *parser_alloc_lock()
{
    return nullptr;
}

void parser_lock(void *lock)
{
}

void parser_unlock(void *lock)
{
}

void parser_free_lock(void *lock)
{
}

void parser_hello(void *priv, struct usb_redir_hello_header *h)
{
    log("%s: %s\n", __func__, h->version);
}

void parser_device_disconnect_ack(void *priv)
{
}

void parser_start_bulk_receiving(void *priv, uint64_t id,
    struct usb_redir_start_bulk_receiving_header *start_bulk_receiving)
{
}

void parser_stop_bulk_receiving(void *priv, uint64_t id,
    struct usb_redir_stop_bulk_receiving_header *stop_bulk_receiving)
{
}

void parser_bulk_receiving_status(void *priv, uint64_t id,
    struct usb_redir_bulk_receiving_status_header *bulk_receiving_status)
{
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::array<uint32_t, USB_REDIR_CAPS_SIZE> caps = {0};
    int ret;

    fdp = std::make_unique<FuzzedDataProvider>(data, size);

    parser.reset(usbredirparser_create());
    if (parser == nullptr) {
        return 1;
    }

    parser->log_func = parser_log;
    parser->read_func = parser_read;
    parser->write_func = parser_write;
    parser->device_connect_func = parser_device_connect;
    parser->device_disconnect_func = parser_device_disconnect;
    parser->reset_func = parser_reset;
    parser->interface_info_func = parser_interface_info;
    parser->ep_info_func = parser_ep_info;
    parser->set_configuration_func = parser_set_configuration;
    parser->get_configuration_func = parser_get_configuration;
    parser->configuration_status_func = parser_configuration_status;
    parser->set_alt_setting_func = parser_set_alt_setting;
    parser->get_alt_setting_func = parser_get_alt_setting;
    parser->alt_setting_status_func = parser_alt_setting_status;
    parser->start_iso_stream_func = parser_start_iso_stream;
    parser->stop_iso_stream_func = parser_stop_iso_stream;
    parser->iso_stream_status_func = parser_iso_stream_status;
    parser->start_interrupt_receiving_func = parser_start_interrupt_receiving;
    parser->stop_interrupt_receiving_func = parser_stop_interrupt_receiving;
    parser->interrupt_receiving_status_func = parser_interrupt_receiving_status;
    parser->alloc_bulk_streams_func = parser_alloc_bulk_streams;
    parser->free_bulk_streams_func = parser_free_bulk_streams;
    parser->bulk_streams_status_func = parser_bulk_streams_status;
    parser->cancel_data_packet_func = parser_cancel_data_packet;
    parser->control_packet_func = parser_control_packet;
    parser->bulk_packet_func = parser_bulk_packet;
    parser->iso_packet_func = parser_iso_packet;
    parser->interrupt_packet_func = parser_interrupt_packet;
    parser->alloc_lock_func = parser_alloc_lock;
    parser->lock_func = parser_lock;
    parser->unlock_func = parser_unlock;
    parser->free_lock_func = parser_free_lock;
    parser->hello_func = parser_hello;
    parser->filter_reject_func = parser_filter_reject;
    parser->filter_filter_func = parser_filter_filter;
    parser->device_disconnect_ack_func = parser_device_disconnect_ack;
    parser->start_bulk_receiving_func = parser_start_bulk_receiving;
    parser->stop_bulk_receiving_func = parser_stop_bulk_receiving;
    parser->bulk_receiving_status_func = parser_bulk_receiving_status;
    parser->buffered_bulk_packet_func = parser_buffered_bulk_packet;

    for (uint32_t &cap : caps) {
        cap = fdp->ConsumeIntegral<decltype(caps)::value_type>();
    }

    int init_flags = 0;

    if (fdp->ConsumeBool()) {
        init_flags |= usbredirparser_fl_usb_host;
    }

    usbredirparser_init(parser.get(), "fuzzer", caps.data(), caps.size(),
                        init_flags);

    while (fdp->remaining_bytes() > 0) {
        ret = usbredirparser_do_read(parser.get());
        if (ret != 0) {
            log("usbredirparser_do_read failed: %d\n", ret);
            goto out;
        }

        while (usbredirparser_has_data_to_write(parser.get())) {
            ret = usbredirparser_do_write(parser.get());
            if (ret != 0) {
                log("usbredirparser_do_write failed: %d\n", ret);
                goto out;
            }
        }
    }

out:
    parser.reset();

    return 0;
}

/* vim: set sw=4 sts=4 et : */
