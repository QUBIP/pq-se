/**
  * @file demo_aes.c
  * @brief AES Validation Code
  *
  * @section License
  *
  * Secure Element for QUBIP Project
  *
  * This Secure Element repository for QUBIP Project is subject to the
  * BSD 3-Clause License below.
  *
  * Copyright (c) 2024,
  *         Eros Camacho-Ruiz
  *         Pablo Navarro-Torrero
  *         Pau Ortega-Castro
  *         Apurba Karmakar
  *         Macarena C. Martínez-Rodríguez
  *         Piedad Brox
  *
  * All rights reserved.
  *
  * This Secure Element was developed by Instituto de Microelectrónica de
  * Sevilla - IMSE (CSIC/US) as part of the QUBIP Project, co-funded by the
  * European Union under the Horizon Europe framework programme
  * [grant agreement no. 101119746].
  *
  * -----------------------------------------------------------------------
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice, this
  *    list of conditions and the following disclaimer.
  *
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  *
  * 3. Neither the name of the copyright holder nor the names of its
  *    contributors may be used to endorse or promote products derived from
  *    this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  *
  *
  *
  * @author Eros Camacho-Ruiz (camacho@imse-cnm.csic.es)
  * @version 1.0
  **/

#include "demo.h"
#include "test_func.h"
 

void demo_aes_hw(unsigned int bits, unsigned int verb, INTF interface) {

#ifdef AXI
    unsigned int clk_index = 0;
    float clk_frequency;
    float set_clk_frequency = FREQ_AES;
    Set_Clk_Freq(clk_index, &clk_frequency, &set_clk_frequency, (int)verb);
#endif 

    unsigned char msg[128] = "Hello, this is the SE of QUBIP project";

    // ---- AES-128 ---- //
    if (bits == 128) {
        unsigned char* recovered_msg_128;
        unsigned int recovered_msg_128_len;

        unsigned char* char_key_128 = "2b7e151628aed2a6abf7158809cf4f3c";
        unsigned char key_128[16]; char2hex(char_key_128, key_128);
        unsigned char* char_iv_128 = "000102030405060708090a0b0c0d0e0f";
        unsigned char iv_128[16]; char2hex(char_iv_128, iv_128);
        unsigned char* char_add_128 = "000102030405060708090a0b0c0d0e0f";
        unsigned char add_128[16]; char2hex(char_add_128, add_128);

        unsigned char* ciphertext_128;
        unsigned int ciphertext_128_len;

        ciphertext_128 = malloc(256); memset(ciphertext_128, 0, 256); // It is neccesary to add some bytes more
        recovered_msg_128 = malloc(128); memset(recovered_msg_128, 0, 128);

        // --- ECB --- //
        aes_128_ecb_encrypt_hw(key_128, ciphertext_128, &ciphertext_128_len, msg, 128, interface);
        aes_128_ecb_decrypt_hw(key_128, ciphertext_128, ciphertext_128_len, recovered_msg_128, &recovered_msg_128_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_128);

        print_result_valid("AES-128-ECB", memcmp(msg, recovered_msg_128, 128));


        // --- CBC --- //
        aes_128_cbc_encrypt_hw(key_128, iv_128, ciphertext_128, &ciphertext_128_len, msg, 128, interface);
        aes_128_cbc_decrypt_hw(key_128, iv_128, ciphertext_128, ciphertext_128_len, recovered_msg_128, &recovered_msg_128_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_128);

        print_result_valid("AES-128-CBC", memcmp(msg, recovered_msg_128, 128));

        // --- CMAC --- //
        unsigned char* char_exp_mac_128 = "c430faa66e964f24a466a2fe00ff044c";
        unsigned char exp_mac_128[16]; char2hex(char_exp_mac_128, exp_mac_128);
        unsigned char* mac_128;
        unsigned int mac_128_len;
        mac_128 = malloc(16); memset(mac_128, 0, 16);

        aes_128_cmac_hw(key_128, mac_128, &mac_128_len, msg, 128, interface);

        if (verb >= 1) {
            printf("\n Obtained Result: ");  show_array(mac_128, 16, 32);
            printf("\n Expected Result: ");  show_array(exp_mac_128, 16, 32);
        }

        print_result_valid("AES-128-CMAC", memcmp(exp_mac_128, mac_128, 16));

        free(mac_128);

        // --- GCM --- //
        unsigned char tag[16];
        unsigned int result = 1;
        aes_128_gcm_encrypt_hw(key_128, iv_128, 16, ciphertext_128, &ciphertext_128_len, msg, 128, add_128, 16, tag, interface); 
        aes_128_gcm_decrypt_hw(key_128, iv_128, 16, ciphertext_128, ciphertext_128_len, recovered_msg_128, &recovered_msg_128_len, add_128, 16, tag, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_128);
        if (verb >= 1) { printf("\n tag: "); show_array(tag, 16, 32); }

        print_result_valid("AES-128-GCM", result);

        // --- CCM_8 --- //
        unsigned char tag_8[8]; memset(tag_8, 0, 8);
        result = 1;
        aes_128_ccm_8_encrypt_hw(key_128, iv_128, 8, ciphertext_128, &ciphertext_128_len, msg, 128, add_128, 16, tag_8, interface);
        aes_128_ccm_8_decrypt_hw(key_128, iv_128, 8, ciphertext_128, ciphertext_128_len, recovered_msg_128, &recovered_msg_128_len, add_128, 16, tag_8, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_128);
        if (verb >= 1) { printf("\n tag: "); show_array(tag_8, 8, 32); }

        print_result_valid("AES-128-CCM-8", result);

        free(ciphertext_128);
        free(recovered_msg_128);

    }
    // ---- AES-192 ---- //
    else if (bits == 192) {
        unsigned char* recovered_msg_192;
        unsigned int recovered_msg_192_len;

        unsigned char* char_key_192 = "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b";
        unsigned char key_192[24]; char2hex(char_key_192, key_192);
        unsigned char* char_iv_192 = "000102030405060708090a0b0c0d0e0f";
        unsigned char iv_192[16]; char2hex(char_iv_192, iv_192);
        unsigned char* char_add_192 = "000102030405060708090a0b0c0d0e0f";
        unsigned char add_192[16]; char2hex(char_add_192, add_192);

        unsigned char* ciphertext_192;
        unsigned int ciphertext_192_len;

        ciphertext_192 = malloc(256); memset(ciphertext_192, 0, 256);
        recovered_msg_192 = malloc(128); memset(recovered_msg_192, 0, 128);

        // --- ECB --- //
        aes_192_ecb_encrypt_hw(key_192, ciphertext_192, &ciphertext_192_len, msg, 128, interface);
        aes_192_ecb_decrypt_hw(key_192, ciphertext_192, ciphertext_192_len, recovered_msg_192, &recovered_msg_192_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_192);

        print_result_valid("AES-192-ECB", memcmp(msg, recovered_msg_192, 128));


        // --- CBC --- //
        aes_192_cbc_encrypt_hw(key_192, iv_192, ciphertext_192, &ciphertext_192_len, msg, 128, interface);
        aes_192_cbc_decrypt_hw(key_192, iv_192, ciphertext_192, ciphertext_192_len, recovered_msg_192, &recovered_msg_192_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_192);

        print_result_valid("AES-192-CBC", memcmp(msg, recovered_msg_192, 128));

        // --- CMAC --- //
        unsigned char* char_exp_mac_192 = "fcb99201cb9804df7bf985377f075711";
        unsigned char exp_mac_192[16]; char2hex(char_exp_mac_192, exp_mac_192);
        unsigned char* mac_192;
        unsigned int mac_192_len;
        mac_192 = calloc(sizeof(char), 16);

        aes_192_cmac_hw(key_192, mac_192, &mac_192_len, msg, 128, interface);

        if (verb >= 1) {
            printf("\n Obtained Result: ");  show_array(mac_192, 16, 32);
            printf("\n Expected Result: ");  show_array(exp_mac_192, 16, 32);
        }

        print_result_valid("AES-192-CMAC", memcmp(exp_mac_192, mac_192, 16));

        free(mac_192);

        // --- GCM --- //
        unsigned char tag[16]; memset(tag, 0, 16);
        unsigned int result = 1;
        aes_192_gcm_encrypt_hw(key_192, iv_192, 16, ciphertext_192, &ciphertext_192_len, msg, 128, add_192, 16, tag, interface);
        aes_192_gcm_decrypt_hw(key_192, iv_192, 16, ciphertext_192, ciphertext_192_len, recovered_msg_192, &recovered_msg_192_len, add_192, 16, tag, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_192);
        if (verb >= 1) { printf("\n tag: "); show_array(tag, 16, 32); }

        print_result_valid("AES-192-GCM", result);

        // --- CCM_8 --- //
        unsigned char tag_8[8]; memset(tag_8, 0, 8);
        result = 1;
        aes_192_ccm_8_encrypt_hw(key_192, iv_192, 8, ciphertext_192, &ciphertext_192_len, msg, 128, add_192, 16, tag_8, interface);
        aes_192_ccm_8_decrypt_hw(key_192, iv_192, 8, ciphertext_192, ciphertext_192_len, recovered_msg_192, &recovered_msg_192_len, add_192, 16, tag_8, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_192);
        if (verb >= 1) { printf("\n tag: "); show_array(tag_8, 8, 32); }

        print_result_valid("AES-192-CCM-8", result);

        free(ciphertext_192);
        free(recovered_msg_192);
    }
    else {
        unsigned char* recovered_msg_256;
        unsigned int recovered_msg_256_len;

        unsigned char* char_key_256 = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4";
        unsigned char key_256[32]; char2hex(char_key_256, key_256);
        unsigned char* char_iv_256 = "000102030405060708090a0b0c0d0e0f";
        unsigned char iv_256[16]; char2hex(char_iv_256, iv_256);
        unsigned char* char_add_256 = "000102030405060708090a0b0c0d0e0f";
        unsigned char add_256[16]; char2hex(char_add_256, add_256);

        unsigned char* ciphertext_256;
        unsigned int ciphertext_256_len;

        ciphertext_256 = malloc(256); memset(ciphertext_256, 0, 256);
        recovered_msg_256 = malloc(128); memset(recovered_msg_256, 0, 128);

        // --- ECB --- //
        aes_256_ecb_encrypt_hw(key_256, ciphertext_256, &ciphertext_256_len, msg, 128, interface);
        aes_256_ecb_decrypt_hw(key_256, ciphertext_256, ciphertext_256_len, recovered_msg_256, &recovered_msg_256_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_256);

        print_result_valid("AES-256-ECB", memcmp(msg, recovered_msg_256, 128));

        // --- CBC --- //
        aes_256_cbc_encrypt_hw(key_256, iv_256, ciphertext_256, &ciphertext_256_len, msg, 128, interface);
        aes_256_cbc_decrypt_hw(key_256, iv_256, ciphertext_256, ciphertext_256_len, recovered_msg_256, &recovered_msg_256_len, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_256);

        print_result_valid("AES-256-CBC", memcmp(msg, recovered_msg_256, 128));

        // --- CMAC --- //
        unsigned char* char_exp_mac_256 = "7a88b15caef3b438ddbb0299a51d70d3";
        unsigned char exp_mac_256[16]; char2hex(char_exp_mac_256, exp_mac_256);
        unsigned char* mac_256;
        unsigned int mac_256_len;
        mac_256 = calloc(sizeof(char), 16);

        aes_256_cmac_hw(key_256, mac_256, &mac_256_len, msg, 128, interface);

        if (verb >= 1) {
            printf("\n Obtained Result: ");  show_array(mac_256, 16, 32);
            printf("\n Expected Result: ");  show_array(exp_mac_256, 16, 32);
        }

        print_result_valid("AES-256-CMAC", memcmp(exp_mac_256, mac_256, 16));

        free(mac_256);

        // --- GCM --- //
        unsigned char tag[16]; memset(tag, 0, 16);
        unsigned int result = 1;
        aes_256_gcm_encrypt_hw(key_256, iv_256, 16, ciphertext_256, &ciphertext_256_len, msg, 128, add_256, 16, tag, interface);
        aes_256_gcm_decrypt_hw(key_256, iv_256, 16, ciphertext_256, ciphertext_256_len, recovered_msg_256, &recovered_msg_256_len, add_256, 16, tag, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_256);
        if (verb >= 1) { printf("\n tag: "); show_array(tag, 16, 32); }

        print_result_valid("AES-256-GCM", result);

        // --- CCM_8 --- //
        unsigned char tag_8[8]; memset(tag_8, 0, 8);
        result = 1;
        aes_256_ccm_8_encrypt_hw(key_256, iv_256, 8, ciphertext_256, &ciphertext_256_len, msg, 128, add_256, 16, tag_8, interface);
        aes_256_ccm_8_decrypt_hw(key_256, iv_256, 8, ciphertext_256, ciphertext_256_len, recovered_msg_256, &recovered_msg_256_len, add_256, 16, tag_8, &result, interface);

        if (verb >= 1) printf("\n original msg: %s", msg);
        if (verb >= 1) printf("\n recover msg: %s", recovered_msg_256);
        if (verb >= 1) { printf("\n tag: "); show_array(tag_8, 8, 32); }

        print_result_valid("AES-256-CCM-8", result);
    
        free(ciphertext_256);
        free(recovered_msg_256);

    }
#ifdef AXI
    set_clk_frequency = FREQ_TYPICAL;
    Set_Clk_Freq(clk_index, &clk_frequency, &set_clk_frequency, (int)verb);
#endif


}