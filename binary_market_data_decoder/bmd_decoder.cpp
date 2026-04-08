#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <inttypes.h>

static uint64_t read_u64(const uint8_t* p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) v = (v << 8) | p[i];
    return v;
}

static uint32_t read_u32(const uint8_t* p) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) v = (v << 8) | p[i];
    return v;
}

static uint16_t read_u16(const uint8_t* p) {
    return (p[0] << 8) | p[1];
}

static void read_str(const uint8_t* p, int len, char* out) {
    memcpy(out, p, len);
    out[len] = '\0';
    for (int i = len - 1; i >= 0 && out[i] == ' '; i--)
        out[i] = '\0';
}

static void print_separator() {
    printf("────────────────────────────────────────────────\n");
}

static void print_hex(const uint8_t* data, int len) {
    printf("  Hex: ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0 && i + 1 < len)
            printf("\n       ");
    }
    printf("\n");
}

// MsgDecoder
static int decode_message(const uint8_t* data, int remaining, int msg_num,
                          const char* session, uint64_t seq_num) {
    if (remaining < 1) return 0;
    char type = (char)data[0];
    int consumed = 0;

    printf("  Message #%d  Type: '%c'", msg_num, type);

    switch (type) {
    case 'S': {
        if (remaining < 14) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t ts = read_u64(data + 1);
        char mkt[5]; read_str(data + 9, 4, mkt);
        char evt = (char)data[13];
        printf("  (SystemEvent)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  Timestamp       : %" PRIu64 "\n", ts);
        printf("  MarketCode      : %s\n", mkt);
        printf("  SystemEvent     : '%c'\n", evt);
        consumed = 14;
        print_hex(data, consumed);
        break;
    }
    case 'R': {
        if (remaining < 59) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t ts = read_u64(data + 1);
        char secid[5]; read_str(data + 9, 4, secid);
        char isin[13]; read_str(data + 13, 12, isin);
        char mkt[5]; read_str(data + 25, 4, mkt);
        uint32_t lot = read_u32(data + 29);
        uint8_t dec = data[33];
        char state = (char)data[34];
        uint64_t ref = read_u64(data + 35);
        uint64_t upper = read_u64(data + 43);
        uint64_t lower = read_u64(data + 51);
        printf("  (ReferencePrice)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  Timestamp       : %" PRIu64 "\n", ts);
        printf("  SecurityId      : %s\n", secid);
        printf("  ISINCode        : %s\n", isin);
        printf("  MarketCode      : %s\n", mkt);
        printf("  RoundLotSize    : %u\n", lot);
        printf("  PriceDecimals   : %u\n", dec);
        printf("  TradingState    : '%c'\n", state);
        printf("  ReferencePrice  : %" PRIu64 "\n", ref);
        printf("  UpperPriceLimit : %" PRIu64 "\n", upper);
        printf("  LowerPriceLimit : %" PRIu64 "\n", lower);
        consumed = 59;
        print_hex(data, consumed);
        break;
    }
    case 'H': {
        if (remaining < 18) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t ts = read_u64(data + 1);
        char secid[5]; read_str(data + 9, 4, secid);
        char mkt[5]; read_str(data + 13, 4, mkt);
        char state = (char)data[17];
        printf("  (TradingStatus)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  Timestamp       : %" PRIu64 "\n", ts);
        printf("  SecurityId      : %s\n", secid);
        printf("  MarketCode      : %s\n", mkt);
        printf("  TradingState    : '%c'\n", state);
        consumed = 18;
        print_hex(data, consumed);
        break;
    }
    case 'J': {
        if (remaining < 33) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t ts = read_u64(data + 1);
        char secid[5]; read_str(data + 9, 4, secid);
        char mkt[5]; read_str(data + 13, 4, mkt);
        uint64_t ref = read_u64(data + 17);
        uint64_t upper = read_u64(data + 25);
        uint64_t lower = read_u64(data + 33);
        printf("  (PriceLimitUpdate)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  Timestamp       : %" PRIu64 "\n", ts);
        printf("  SecurityId      : %s\n", secid);
        printf("  MarketCode      : %s\n", mkt);
        printf("  ReferencePrice  : %" PRIu64 "\n", ref);
        printf("  UpperPriceLimit : %" PRIu64 "\n", upper);
        printf("  LowerPriceLimit : %" PRIu64 "\n", lower);
        consumed = 33;
        print_hex(data, consumed);
        break;
    }
    case 'P': {
        if (remaining < 48) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t ts = read_u64(data + 1);
        char secid[5]; read_str(data + 9, 4, secid);
        char mkt[5]; read_str(data + 13, 4, mkt);
        uint32_t tdate = read_u32(data + 17);
        uint8_t sdate = data[21];
        char ttype = (char)data[22];
        char ptype = (char)data[23];
        uint64_t qty = read_u64(data + 24);
        uint64_t price = read_u64(data + 32);
        uint64_t match = read_u64(data + 40);
        printf("  (Trade)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  Timestamp       : %" PRIu64 "\n", ts);
        printf("  SecurityId      : %s\n", secid);
        printf("  MarketCode      : %s\n", mkt);
        printf("  TradeDate       : %u\n", tdate);
        printf("  SettleDate      : %u\n", sdate);
        printf("  TradeType       : '%c'\n", ttype);
        printf("  PriceType       : '%c'\n", ptype);
        printf("  ExecutedQuantity: %" PRIu64 "\n", qty);
        printf("  ExecutionPrice  : %" PRIu64 "\n", price);
        printf("  MatchNumber     : %" PRIu64 "\n", match);
        consumed = 48;
        print_hex(data, consumed);
        break;
    }
    case 'G': {
        if (remaining < 9) { printf(" [TRUNCATED]\n"); return 0; }
        uint64_t seq = read_u64(data + 1);
        printf("  (SequenceReset)\n");
        print_separator();
        printf("  Session         : %s\n", session);
        printf("  SequenceNumber  : %" PRIu64 "\n", seq_num);
        printf("  ResetSeqNumber  : %" PRIu64 "\n", seq);
        consumed = 9;
        print_hex(data, consumed);
        break;
    }
    default:
        printf("  (Unknown type 0x%02X)\n", (uint8_t)type);
        return 0;
    }

    printf("\n");
    return consumed;
}

// PCAP File parser
int decode_pcap(const char* filename, int max_packets, int skip_moldudp) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: cannot open '%s'\n", filename);
        return 1;
    }

    uint8_t ghdr[24];
    if (fread(ghdr, 1, 24, fp) != 24) {
        fprintf(stderr, "Error: invalid pcap file\n");
        fclose(fp);
        return 1;
    }

    uint32_t magic = *(uint32_t*)ghdr;
    int nano_ts = 0;
    if (magic == 0xa1b23c4d || magic == 0x4d3cb2a1) {
        nano_ts = 1;
    }
    int swap = (magic == 0xd4c3b2a1 || magic == 0x4d3cb2a1);

    int pkt_num = 0;
    uint8_t phdr[16];

    while (fread(phdr, 1, 16, fp) == 16) {
        uint32_t ts_sec, ts_frac, incl_len, orig_len;
        if (swap) {
            ts_sec   = __builtin_bswap32(*(uint32_t*)(phdr));
            ts_frac  = __builtin_bswap32(*(uint32_t*)(phdr+4));
            incl_len = __builtin_bswap32(*(uint32_t*)(phdr+8));
            orig_len = __builtin_bswap32(*(uint32_t*)(phdr+12));
        } else {
            ts_sec   = *(uint32_t*)(phdr);
            ts_frac  = *(uint32_t*)(phdr+4);
            incl_len = *(uint32_t*)(phdr+8);
            orig_len = *(uint32_t*)(phdr+12);
        }

        uint8_t* pkt = (uint8_t*)malloc(incl_len);
        if (!pkt || fread(pkt, 1, incl_len, fp) != incl_len) {
            free(pkt);
            break;
        }

        pkt_num++;
        if (max_packets > 0 && pkt_num > max_packets) {
            free(pkt);
            break;
        }

        time_t t = ts_sec + 9 * 3600;
        struct tm tm;
        gmtime_r(&t, &tm);

        printf("════════════════════════════════════════════════\n");
        printf("Packet #%d  Capture Time: %02d:%02d:%02d.%0*u JST  Size: %u bytes\n",
               pkt_num, tm.tm_hour, tm.tm_min, tm.tm_sec,
               nano_ts ? 9 : 6, ts_frac, orig_len);

        if (incl_len >= 42) {
            const uint8_t* ip = pkt + 14;
            const uint8_t* udp = pkt + 34;
            uint16_t src_port = read_u16(udp);
            uint16_t dst_port = read_u16(udp + 2);
            printf("  %u.%u.%u.%u:%u > %u.%u.%u.%u:%u (UDP)\n",
                   ip[12], ip[13], ip[14], ip[15], src_port,
                   ip[16], ip[17], ip[18], ip[19], dst_port);
        }
        printf("════════════════════════════════════════════════\n");

        int offset = 42;

        if (skip_moldudp) {
            if ((int)incl_len >= offset + 20) {
                // Parse MoldUDP64 header
                char session[11];
                memcpy(session, pkt + offset, 10);
                session[10] = '\0';
                uint64_t base_seq = read_u64(pkt + offset + 10);
                uint16_t msg_count = read_u16(pkt + offset + 18);
                offset += 20;

                if (msg_count == 0) {
                    printf("  (Heartbeat)\n\n");
                    free(pkt);
                    continue;
                }

                for (uint16_t m = 0; m < msg_count && offset < (int)incl_len; m++) {
                    if (offset + 2 > (int)incl_len) break;
                    uint16_t msg_len = read_u16(pkt + offset);
                    offset += 2;
                    if (msg_len == 0 || offset + msg_len > (int)incl_len) break;
                    decode_message(pkt + offset, msg_len, m + 1, session, base_seq + m);
                    offset += msg_len;
                }
            }
        } else {
            int msg_num = 0;
            while (offset < (int)incl_len) {
                msg_num++;
                int consumed = decode_message(pkt + offset, incl_len - offset, msg_num, "", 0);
                if (consumed == 0) break;
                offset += consumed;
            }
        }

        free(pkt);
    }

    fclose(fp);
    printf("Total packets: %d\n", pkt_num);
    return 0;
}

void print_usage(const char* prog) {
    printf("Binary Market Data Decoder (.pcap)\n\n");
    printf("Usage: %s [OPTIONS] <pcap_file>\n\n", prog);
    printf("Options:\n");
    printf("  -n NUM     Decode first NUM packets only (default: all)\n");
    printf("  -m         Parse MoldUDP64 transport header\n");
    printf("  -h         Show this help\n\n");
    printf("Message types: S=SystemEvent, R=ReferencePrice, H=TradingStatus,\n");
    printf("               J=PriceLimitUpdate, P=Trade, G=SequenceReset\n\n");
    printf("Examples:\n");
    printf("  %s capture.pcap\n", prog);
    printf("  %s -m capture.pcap\n", prog);
    printf("  %s -m -n 10 capture.pcap\n", prog);
}

int main(int argc, char* argv[]) {
    int max_packets = 0;
    int skip_moldudp = 0;
    const char* filename = nullptr;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            max_packets = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-m") == 0) {
            skip_moldudp = 1;
        } else {
            filename = argv[i];
        }
    }

    if (!filename) {
        print_usage(argv[0]);
        return 1;
    }

    return decode_pcap(filename, max_packets, skip_moldudp);
}
