/*
 * Copyright 2012 Ayla Networks, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Ayla Networks, Inc.
 */
#ifndef __SPI_PING_H__
#define __SPI_PING_H__

/*
 * In spi_ping.c.
 */

/*
 * Handle received ping packet
 * Fill in spi_ping_result with info from packet.
 */
void spi_ping_rx(void *buf, size_t len);

/*
 * Send test-pattern packet over SPI and read it back.
 *
 * The test pattern is a ping with data with a 2-byte counting pattern.
 *
 * len is the length in the range of 3 to 384 bytes, inclusive.
 * pattern_start is the first 2-byte value of the pattern to be used.
 *
 * This function calles spi_poll() to let the SPI packet state machine send
 * and receive other packets that may be required by the module, so other
 * callbacks, for example to receive property settings, may be called during
 * this function.
 *
 * Returns 0 on success, positive error count otherwise,
 * -1 on invalid length, -2 if busy or not ready.
 */
int spi_ping_test(size_t len, u16 pattern);

#endif /* __SPI_PING_H__ */
