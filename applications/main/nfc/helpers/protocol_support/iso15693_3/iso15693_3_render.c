#include "iso15693_3_render.h"

#define NFC_RENDER_ISO15693_3_MAX_BYTES (128U)

void nfc_render_iso15693_3_info(
    const Iso15693_3Data* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat(str, "ISO15693-3 (NFC-V)\n");
    }

    nfc_render_iso15693_3_brief(data, str);

    if(format_type == NfcProtocolFormatTypeFull) {
        nfc_render_iso15693_3_extra(data, str);
    }
}

void nfc_render_iso15693_3_brief(const Iso15693_3Data* data, FuriString* str) {
    furi_string_cat_printf(str, "UID:\n");

    size_t uid_len;
    const uint8_t* uid = iso15693_3_get_uid(data, &uid_len);

    for(size_t i = 0; i < uid_len; i++) {
        furi_string_cat_printf(str, "%02X ", uid[i]);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
        const uint16_t block_count = iso15693_3_get_block_count(data);
        const uint8_t block_size = iso15693_3_get_block_size(data);

        furi_string_cat_printf(str, "\nMémoire: %u bytes\n", block_count * block_size);
        furi_string_cat_printf(str, "(%u blocs x %u bytes)", block_count, block_size);
    }
}

void nfc_render_iso15693_3_system_info(const Iso15693_3Data* data, FuriString* str) {
    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
        furi_string_cat(str, "\e#Données mémoire\n\e*--------------------\n");

        const uint16_t block_count = iso15693_3_get_block_count(data);
        const uint8_t block_size = iso15693_3_get_block_size(data);
        const uint16_t display_block_count =
            MIN(NFC_RENDER_ISO15693_3_MAX_BYTES / block_size, block_count);

        for(uint32_t i = 0; i < display_block_count; ++i) {
            furi_string_cat(str, "\e*");

            const uint8_t* block_data = iso15693_3_get_block_data(data, i);
            for(uint32_t j = 0; j < block_size; ++j) {
                furi_string_cat_printf(str, "%02X ", block_data[j]);
            }

            const char* lock_str = iso15693_3_is_block_locked(data, i) ? "[LOCK]" : "";
            furi_string_cat_printf(str, "| %s\n", lock_str);
        }

        if(block_count != display_block_count) {
            furi_string_cat_printf(
                str,
                "(Données volumineuses. Afficher les %u premiers octets.)",
                display_block_count * block_size);
        }
    } else {
        furi_string_cat(str, "\e#No available data\n");
    }
}

void nfc_render_iso15693_3_extra(const Iso15693_3Data* data, FuriString* str) {
    furi_string_cat(str, "\n::::::::::::::::[Infos générales]:::::::::::::::::\n");
    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
        furi_string_cat_printf(str, "DSFID: %02X\n", data->system_info.ic_ref);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_AFI) {
        furi_string_cat_printf(str, "AFI: %02X\n", data->system_info.afi);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
        furi_string_cat_printf(str, "Référence IC: %02X\n", data->system_info.ic_ref);
    }

    furi_string_cat(str, ":::::::::::::::::::[Lock bits]::::::::::::::::::::\n");

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
        furi_string_cat_printf(
            str, "DSFID: %s bloqué\n", data->settings.lock_bits.dsfid ? "" : "non");
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_AFI) {
        furi_string_cat_printf(
            str, "AFI: %s bloqué\n", data->settings.lock_bits.dsfid ? "" : "non");
    }
}
