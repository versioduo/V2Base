// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#include "Base64.h"

static uint8_t b64Encode(uint8_t v) {
  if (v < 26)
    return v + 'A';

  if (v < 52)
    return v + 71;

  if (v < 62)
    return v - 4;

  if (v == 62)
    return '+';

  if (v == 63)
    return '/';

  return 64;
}

uint32_t V2Base::Text::Base64::encode(const uint8_t input[], uint32_t length, uint8_t output[]) {
  uint32_t fullSets = length / 3;
  for (uint32_t i = 0; i < fullSets; ++i) {
    output[0] = b64Encode(input[0] >> 2);
    output[1] = b64Encode((input[0] & 0x03) << 4 | input[1] >> 4);
    output[2] = b64Encode((input[1] & 0x0F) << 2 | input[2] >> 6);
    output[3] = b64Encode(input[2] & 0x3F);
    input += 3;
    output += 4;
  }

  switch (length % 3) {
    case 0:
      output[0] = '\0';
      break;

    case 1:
      output[0] = b64Encode(input[0] >> 2);
      output[1] = b64Encode((input[0] & 0x03) << 4);
      output[2] = '=';
      output[3] = '=';
      output[4] = '\0';
      break;

    case 2:
      output[0] = b64Encode(input[0] >> 2);
      output[1] = b64Encode((input[0] & 0x03) << 4 | input[1] >> 4);
      output[2] = b64Encode((input[1] & 0x0F) << 2);
      output[3] = '=';
      output[4] = '\0';
      break;
  }

  return (length + 2) / 3 * 4;
}

static uint8_t decodeB64(uint8_t c) {
  if ('A' <= c && c <= 'Z')
    return c - 'A';

  if ('a' <= c && c <= 'z')
    return c - 71;

  if ('0' <= c && c <= '9')
    return c + 4;

  if (c == '+')
    return 62;

  if (c == '/')
    return 63;

  return 255;
}

static uint32_t decodeLength(const uint8_t input[]) {
  const uint8_t *start = input;

  while (decodeB64(input[0]) < 64)
    input++;

  uint32_t inputLength  = input - start;
  uint32_t outputLength = inputLength / 4 * 3;

  switch (inputLength % 4) {
    case 2:
      return outputLength + 1;

    case 3:
      return outputLength + 2;

    default:
      return outputLength;
  }
}

uint32_t V2Base::Text::Base64::decode(const uint8_t input[], uint8_t output[]) {
  uint32_t length = decodeLength(input);

  for (uint32_t i = 2; i < length; i += 3) {
    output[0] = decodeB64(input[0]) << 2 | decodeB64(input[1]) >> 4;
    output[1] = decodeB64(input[1]) << 4 | decodeB64(input[2]) >> 2;
    output[2] = decodeB64(input[2]) << 6 | decodeB64(input[3]);
    input += 4;
    output += 3;
  }

  switch (length % 3) {
    case 1:
      output[0] = decodeB64(input[0]) << 2 | decodeB64(input[1]) >> 4;
      break;

    case 2:
      output[0] = decodeB64(input[0]) << 2 | decodeB64(input[1]) >> 4;
      output[1] = decodeB64(input[1]) << 4 | decodeB64(input[2]) >> 2;
      break;
  }

  return length;
}
