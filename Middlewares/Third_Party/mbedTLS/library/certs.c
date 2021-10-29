/*
 *  X.509 test certificates
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/certs.h"

#if defined(MBEDTLS_CERTS_C)

#if defined(MBEDTLS_ECDSA_C)
#define TEST_CA_CRT_EC                                                  \
"-----BEGIN CERTIFICATE-----\r\n"                                       \
"MIICUjCCAdegAwIBAgIJAMFD4n5iQ8zoMAoGCCqGSM49BAMCMD4xCzAJBgNVBAYT\r\n"  \
"Ak5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UEAxMTUG9sYXJzc2wgVGVzdCBF\r\n"  \
"QyBDQTAeFw0xMzA5MjQxNTQ5NDhaFw0yMzA5MjIxNTQ5NDhaMD4xCzAJBgNVBAYT\r\n"  \
"Ak5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UEAxMTUG9sYXJzc2wgVGVzdCBF\r\n"  \
"QyBDQTB2MBAGByqGSM49AgEGBSuBBAAiA2IABMPaKzRBN1gvh1b+/Im6KUNLTuBu\r\n"  \
"ww5XUzM5WNRStJGVOQsj318XJGJI/BqVKc4sLYfCiFKAr9ZqqyHduNMcbli4yuiy\r\n"  \
"aY7zQa0pw7RfdadHb9UZKVVpmlM7ILRmFmAzHqOBoDCBnTAdBgNVHQ4EFgQUnW0g\r\n"  \
"JEkBPyvLeLUZvH4kydv7NnwwbgYDVR0jBGcwZYAUnW0gJEkBPyvLeLUZvH4kydv7\r\n"  \
"NnyhQqRAMD4xCzAJBgNVBAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UE\r\n"  \
"AxMTUG9sYXJzc2wgVGVzdCBFQyBDQYIJAMFD4n5iQ8zoMAwGA1UdEwQFMAMBAf8w\r\n"  \
"CgYIKoZIzj0EAwIDaQAwZgIxAMO0YnNWKJUAfXgSJtJxexn4ipg+kv4znuR50v56\r\n"  \
"t4d0PCu412mUC6Nnd7izvtE2MgIxAP1nnJQjZ8BWukszFQDG48wxCCyci9qpdSMv\r\n"  \
"uCjn8pwUOkABXK8Mss90fzCfCEOtIA==\r\n"                                  \
"-----END CERTIFICATE-----\r\n"
const char mbedtls_test_ca_crt_ec[] = TEST_CA_CRT_EC;

const char mbedtls_test_ca_key_ec[] =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"Proc-Type: 4,ENCRYPTED\r\n"
"DEK-Info: DES-EDE3-CBC,307EAB469933D64E\r\n"
"\r\n"
"IxbrRmKcAzctJqPdTQLA4SWyBYYGYJVkYEna+F7Pa5t5Yg/gKADrFKcm6B72e7DG\r\n"
"ihExtZI648s0zdYw6qSJ74vrPSuWDe5qm93BqsfVH9svtCzWHW0pm1p0KTBCFfUq\r\n"
"UsuWTITwJImcnlAs1gaRZ3sAWm7cOUidL0fo2G0fYUFNcYoCSLffCFTEHBuPnagb\r\n"
"a77x/sY1Bvii8S9/XhDTb6pTMx06wzrm\r\n"
"-----END EC PRIVATE KEY-----\r\n";

const char mbedtls_test_ca_pwd_ec[] = "PolarSSLTest";

const char mbedtls_test_srv_crt_ec[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIICHzCCAaWgAwIBAgIBCTAKBggqhkjOPQQDAjA+MQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxHDAaBgNVBAMTE1BvbGFyc3NsIFRlc3QgRUMgQ0EwHhcN\r\n"
"MTMwOTI0MTU1MjA0WhcNMjMwOTIyMTU1MjA0WjA0MQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxEjAQBgNVBAMTCWxvY2FsaG9zdDBZMBMGByqGSM49AgEG\r\n"
"CCqGSM49AwEHA0IABDfMVtl2CR5acj7HWS3/IG7ufPkGkXTQrRS192giWWKSTuUA\r\n"
"2CMR/+ov0jRdXRa9iojCa3cNVc2KKg76Aci07f+jgZ0wgZowCQYDVR0TBAIwADAd\r\n"
"BgNVHQ4EFgQUUGGlj9QH2deCAQzlZX+MY0anE74wbgYDVR0jBGcwZYAUnW0gJEkB\r\n"
"PyvLeLUZvH4kydv7NnyhQqRAMD4xCzAJBgNVBAYTAk5MMREwDwYDVQQKEwhQb2xh\r\n"
"clNTTDEcMBoGA1UEAxMTUG9sYXJzc2wgVGVzdCBFQyBDQYIJAMFD4n5iQ8zoMAoG\r\n"
"CCqGSM49BAMCA2gAMGUCMQCaLFzXptui5WQN8LlO3ddh1hMxx6tzgLvT03MTVK2S\r\n"
"C12r0Lz3ri/moSEpNZWqPjkCMCE2f53GXcYLqyfyJR078c/xNSUU5+Xxl7VZ414V\r\n"
"fGa5kHvHARBPc8YAIVIqDvHH1Q==\r\n"
"-----END CERTIFICATE-----\r\n";

const char mbedtls_test_srv_key_ec[] =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"MHcCAQEEIPEqEyB2AnCoPL/9U/YDHvdqXYbIogTywwyp6/UfDw6noAoGCCqGSM49\r\n"
"AwEHoUQDQgAEN8xW2XYJHlpyPsdZLf8gbu58+QaRdNCtFLX3aCJZYpJO5QDYIxH/\r\n"
"6i/SNF1dFr2KiMJrdw1VzYoqDvoByLTt/w==\r\n"
"-----END EC PRIVATE KEY-----\r\n";

const char mbedtls_test_cli_crt_ec[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIICLDCCAbKgAwIBAgIBDTAKBggqhkjOPQQDAjA+MQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxHDAaBgNVBAMTE1BvbGFyc3NsIFRlc3QgRUMgQ0EwHhcN\r\n"
"MTMwOTI0MTU1MjA0WhcNMjMwOTIyMTU1MjA0WjBBMQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxHzAdBgNVBAMTFlBvbGFyU1NMIFRlc3QgQ2xpZW50IDIw\r\n"
"WTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARX5a6xc9/TrLuTuIH/Eq7u5lOszlVT\r\n"
"9jQOzC7jYyUL35ji81xgNpbA1RgUcOV/n9VLRRjlsGzVXPiWj4dwo+THo4GdMIGa\r\n"
"MAkGA1UdEwQCMAAwHQYDVR0OBBYEFHoAX4Zk/OBd5REQO7LmO8QmP8/iMG4GA1Ud\r\n"
"IwRnMGWAFJ1tICRJAT8ry3i1Gbx+JMnb+zZ8oUKkQDA+MQswCQYDVQQGEwJOTDER\r\n"
"MA8GA1UEChMIUG9sYXJTU0wxHDAaBgNVBAMTE1BvbGFyc3NsIFRlc3QgRUMgQ0GC\r\n"
"CQDBQ+J+YkPM6DAKBggqhkjOPQQDAgNoADBlAjBKZQ17IIOimbmoD/yN7o89u3BM\r\n"
"lgOsjnhw3fIOoLIWy2WOGsk/LGF++DzvrRzuNiACMQCd8iem1XS4JK7haj8xocpU\r\n"
"LwjQje5PDGHfd3h9tP38Qknu5bJqws0md2KOKHyeV0U=\r\n"
"-----END CERTIFICATE-----\r\n";

const char mbedtls_test_cli_key_ec[] =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"MHcCAQEEIPb3hmTxZ3/mZI3vyk7p3U3wBf+WIop6hDhkFzJhmLcqoAoGCCqGSM49\r\n"
"AwEHoUQDQgAEV+WusXPf06y7k7iB/xKu7uZTrM5VU/Y0Dswu42MlC9+Y4vNcYDaW\r\n"
"wNUYFHDlf5/VS0UY5bBs1Vz4lo+HcKPkxw==\r\n"
"-----END EC PRIVATE KEY-----\r\n";

const size_t mbedtls_test_ca_crt_ec_len  = sizeof( mbedtls_test_ca_crt_ec );
const size_t mbedtls_test_ca_key_ec_len  = sizeof( mbedtls_test_ca_key_ec );
const size_t mbedtls_test_ca_pwd_ec_len  = sizeof( mbedtls_test_ca_pwd_ec ) - 1;
const size_t mbedtls_test_srv_crt_ec_len = sizeof( mbedtls_test_srv_crt_ec );
const size_t mbedtls_test_srv_key_ec_len = sizeof( mbedtls_test_srv_key_ec );
const size_t mbedtls_test_cli_crt_ec_len = sizeof( mbedtls_test_cli_crt_ec );
const size_t mbedtls_test_cli_key_ec_len = sizeof( mbedtls_test_cli_key_ec );
#else
#define TEST_CA_CRT_EC
#endif /* MBEDTLS_ECDSA_C */
/*
#if defined(MBEDTLS_RSA_C)
#define TEST_CA_CRT_RSA                                                 \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIE6zCCA9OgAwIBAgIJAIC1+uRGt4jfMA0GCSqGSIb3DQEBCwUAMIGpMQswCQYD\r\n" \
"VQQGEwJVQTELMAkGA1UECBMCUEwxEDAOBgNVBAcTB1BvbHRhdmExEDAOBgNVBAoT\r\n" \
"B0hvbWVTUlYxEDAOBgNVBAsTB0hvbWVTUlYxFjAUBgNVBAMTDWNhLmhvbWVzcnYu\r\n" \
"bnQxHDAaBgNVBCkTE0hvbWVTUlYgMjA0OC1UZXN0Q0ExITAfBgkqhkiG9w0BCQEW\r\n" \
"EmhvbWVzcnZAaG9tZXNydi5udDAeFw0xODA0MjExOTA3MzFaFw0yODA0MTgxOTA3\r\n" \
"MzFaMIGpMQswCQYDVQQGEwJVQTELMAkGA1UECBMCUEwxEDAOBgNVBAcTB1BvbHRh\r\n" \
"dmExEDAOBgNVBAoTB0hvbWVTUlYxEDAOBgNVBAsTB0hvbWVTUlYxFjAUBgNVBAMT\r\n" \
"DWNhLmhvbWVzcnYubnQxHDAaBgNVBCkTE0hvbWVTUlYgMjA0OC1UZXN0Q0ExITAf\r\n" \
"BgkqhkiG9w0BCQEWEmhvbWVzcnZAaG9tZXNydi5udDCCASIwDQYJKoZIhvcNAQEB\r\n" \
"BQADggEPADCCAQoCggEBAJi+tWcgYKhdvx9xY8hzBabPppS0z3cIqfVXqEhJxxFy\r\n" \
"30XL2Z45FYVSfGVKmXPosNntH579bdR3OYBqZ1RdhkT2EsqCtWZPBmjYBcesLiyT\r\n" \
"dJ7Rxh5lZV42aFvZ9Y5SmWMd2gZFF+e9nS1OKRsg1kbRdZ8E0qGpxM3G2F85LFCd\r\n" \
"69cU03nUJG8cTNcxtg1yoH5vasmMCccAcOQDfCvLj1nOMi524u+MZ37ccpKStnrq\r\n" \
"2NFTnojR9ZsBPxFMiuWoRr9GclPYBJZirH6LDkr3t1jtro8906zpUqOmjz6zM5e5\r\n" \
"KxdKtb5CLBLQi+MXOOlOWaqSESReRv1lLtPQO1zubFkCAwEAAaOCARIwggEOMB0G\r\n" \
"A1UdDgQWBBSlmtbOzKzHbsSmOQeAGGaROXzbWzCB3gYDVR0jBIHWMIHTgBSlmtbO\r\n" \
"zKzHbsSmOQeAGGaROXzbW6GBr6SBrDCBqTELMAkGA1UEBhMCVUExCzAJBgNVBAgT\r\n" \
"AlBMMRAwDgYDVQQHEwdQb2x0YXZhMRAwDgYDVQQKEwdIb21lU1JWMRAwDgYDVQQL\r\n" \
"EwdIb21lU1JWMRYwFAYDVQQDEw1jYS5ob21lc3J2Lm50MRwwGgYDVQQpExNIb21l\r\n" \
"U1JWIDIwNDgtVGVzdENBMSEwHwYJKoZIhvcNAQkBFhJob21lc3J2QGhvbWVzcnYu\r\n" \
"bnSCCQCAtfrkRreI3zAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAL\r\n" \
"frL3Tx76TqD1CbGVudMU7CY+8joPZMdaGPD4GNaVp0Z2MqzT1p+fCAS3Q/txRXst\r\n" \
"jwX5X6AsWNzrHg+xIHmO/Yqflz+HDXcKaMa8y85/veEqQHkPmJ1/o1S9QsWwFkdA\r\n" \
"LC6E+Bb6LHpb84VCvxgEGMYl9UvOY+G4xq5oQKZyu0s5VhqkNYRWMN6JM1hgOZ0E\r\n" \
"kYzXBHi/EVoJFz+AZerdMrpX+8kc6KsS1QTqcqONwQAUMYk9sRrV94E+cBMoxXbE\r\n" \
"MVZxkyCLiUkgvGUc5683sKNZL20KQlX8jG92t0k4brRIaCPhfzuv/t7Nw/lpbbfH\r\n" \
"BztIXVXPnhd4KIQK3Beu\r\n" \
"-----END CERTIFICATE-----\r\n"*/


#if defined(MBEDTLS_RSA_C)
#define TEST_CA_CRT_RSA   \
"-----BEGIN CERTIFICATE-----\r\n"\
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\r\n"\
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\r\n"\
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\r\n"\
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\r\n"\
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\r\n"\
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\r\n"\
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\r\n"\
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\r\n"\
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\r\n"\
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\r\n"\
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\r\n"\
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\r\n"\
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\r\n"\
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\r\n"\
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\r\n"\
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\r\n"\
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\r\n"\
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\r\n"\
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\r\n"\
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\r\n"\
"4uJEvlz36hz1\r\n" \
"-----END CERTIFICATE-----\r\n"


const char mbedtls_test_ca_crt_rsa[] = TEST_CA_CRT_RSA;

const char mbedtls_test_ca_key_rsa[] =
"-----BEGIN PRIVATE KEY-----\r\n" \
"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQCYvrVnIGCoXb8f\r\n" \
"cWPIcwWmz6aUtM93CKn1V6hISccRct9Fy9meORWFUnxlSplz6LDZ7R+e/W3UdzmA\r\n" \
"amdUXYZE9hLKgrVmTwZo2AXHrC4sk3Se0cYeZWVeNmhb2fWOUpljHdoGRRfnvZ0t\r\n" \
"TikbINZG0XWfBNKhqcTNxthfOSxQnevXFNN51CRvHEzXMbYNcqB+b2rJjAnHAHDk\r\n" \
"A3wry49ZzjIuduLvjGd+3HKSkrZ66tjRU56I0fWbAT8RTIrlqEa/RnJT2ASWYqx+\r\n" \
"iw5K97dY7a6PPdOs6VKjpo8+szOXuSsXSrW+QiwS0IvjFzjpTlmqkhEkXkb9ZS7T\r\n" \
"0Dtc7mxZAgMBAAECggEBAJdy+IDhTJWEJl3x2wNEcQUd/UiTIuYetA4stRG//V7i\r\n" \
"rFkTL5o/dYQ07lyspAyWWswp3mGRXNX+xQf7+Kpg9SHpb2ZWRbDEiFFqaAi8Qyg1\r\n" \
"qLtUugEnfmP0xHLWYTCV27q4PZZb6cEAdBkY148rdurPArYCXSqoHxf9okqAcN0Y\r\n" \
"wCz2Xq56ZqPBvTWzGjQQqp0tAbGVGCvtcv6GhmVfGWDkrBD/bOfhJ47Az3z8tUkR\r\n" \
"9sWEdoE8v6P2Zz+fQCzdQqaitGebKy58XY/q0yHBz1B+G6uC92fn7DYmWH5E+tH8\r\n" \
"ng07T80uC372SP41jH8F02YuAxqhUjaHIpcHOheo+sUCgYEAyUC0CNHTcGaWg71b\r\n" \
"uTsb1YmyH7uxI1wuLUh7lXv307UeNHkgMxs66W0ShrQwl0Dgc6zV19FE70GlSIkR\r\n" \
"+nwxWU31gC/N1VW5Fe6DyEkKhb5YkwoyNQw24LXeUN+m43xcdjj3ZaYuILQkFvzu\r\n" \
"XqLjPYm6dHXItJO1G/894+ho3kcCgYEAwkvozN/28cIu6cxIi0YArZU3lU49stsD\r\n" \
"4D4i589dnM/ahfUNUuB+1O0vMjAI8Z5isLjdNZc4rp5ztc3gDmV62nA4Nnri5Ed3\r\n" \
"y5CnPcN42k8G4gTD4Cvr83uUGKy1NCrSydPqYZJ6jvDotZHekohPpCe9Jwji60Fy\r\n" \
"fDDTh6kkkF8CgYB3wMWgPeL4DkNf7sxmozKD1SSl11SnetGLZocLQzILyslHZg/v\r\n" \
"+TML9ThB6XTEe/lTO7QfcMn5KHHGgLGvADmJL2d3OHrdh93TnBLUmgnl3YbtL/MC\r\n" \
"N9trySyZqQuwb67BNlKYLNNj6G1VNHpO29QmKQ9G/1NHiGELYM0eEYxdPwKBgQCz\r\n" \
"tpbL+VRmZWATLLrk54j1BZH17fqvlljNb2vF0k4aHz9WrET4pWFq/T+sQjXjlsZe\r\n" \
"A3/Ic+kfyrvoK1bUuhSaOzYjXiLDVz988H1gaY89Dd+l6xZqIPYLrgBbmlO4vutd\r\n" \
"dz/RpDTsRfRZDWU7UA2qHn+cvWF2jICiUl81mcOSLQKBgQC4+Bd/REPqMsPd/NkQ\r\n" \
"MSgV0vJ0ews9fIPfeLKwy6FajIQiRINT8nHA2PLQs0sR6wF1szK86LqZ8BTY11UX\r\n" \
"iprLU9BTptjQmK6hE8iNMKAUz86V+rRx0MtipY997HOj+TGAMgv4bweiiFTlruZL\r\n" \
"r1ugMfnnYu34fwwF0JsbBs1VtQ==\r\n" \
"-----END PRIVATE KEY-----\r\n";

const char mbedtls_test_ca_pwd_rsa[] = "PolarSSLTest";

const char mbedtls_test_srv_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDNzCCAh+gAwIBAgIBAjANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"
"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
"MTEwMjEyMTQ0NDA2WhcNMjEwMjEyMTQ0NDA2WjA0MQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxEjAQBgNVBAMTCWxvY2FsaG9zdDCCASIwDQYJKoZIhvcN\r\n"
"AQEBBQADggEPADCCAQoCggEBAMFNo93nzR3RBNdJcriZrA545Do8Ss86ExbQWuTN\r\n"
"owCIp+4ea5anUrSQ7y1yej4kmvy2NKwk9XfgJmSMnLAofaHa6ozmyRyWvP7BBFKz\r\n"
"NtSj+uGxdtiQwWG0ZlI2oiZTqqt0Xgd9GYLbKtgfoNkNHC1JZvdbJXNG6AuKT2kM\r\n"
"tQCQ4dqCEGZ9rlQri2V5kaHiYcPNQEkI7mgM8YuG0ka/0LiqEQMef1aoGh5EGA8P\r\n"
"hYvai0Re4hjGYi/HZo36Xdh98yeJKQHFkA4/J/EwyEoO79bex8cna8cFPXrEAjya\r\n"
"HT4P6DSYW8tzS1KW2BGiLICIaTla0w+w3lkvEcf36hIBMJcCAwEAAaNNMEswCQYD\r\n"
"VR0TBAIwADAdBgNVHQ4EFgQUpQXoZLjc32APUBJNYKhkr02LQ5MwHwYDVR0jBBgw\r\n"
"FoAUtFrkpbPe0lL2udWmlQ/rPrzH/f8wDQYJKoZIhvcNAQEFBQADggEBAJxnXClY\r\n"
"oHkbp70cqBrsGXLybA74czbO5RdLEgFs7rHVS9r+c293luS/KdliLScZqAzYVylw\r\n"
"UfRWvKMoWhHYKp3dEIS4xTXk6/5zXxhv9Rw8SGc8qn6vITHk1S1mPevtekgasY5Y\r\n"
"iWQuM3h4YVlRH3HHEMAD1TnAexfXHHDFQGe+Bd1iAbz1/sH9H8l4StwX6egvTK3M\r\n"
"wXRwkKkvjKaEDA9ATbZx0mI8LGsxSuCqe9r9dyjmttd47J1p1Rulz3CLzaRcVIuS\r\n"
"RRQfaD8neM9c1S/iJ/amTVqJxA1KOdOS5780WhPfSArA+g4qAmSjelc3p4wWpha8\r\n"
"zhuYwjVuX6JHG0c=\r\n"
"-----END CERTIFICATE-----\r\n";

const char mbedtls_test_srv_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAwU2j3efNHdEE10lyuJmsDnjkOjxKzzoTFtBa5M2jAIin7h5r\r\n"
"lqdStJDvLXJ6PiSa/LY0rCT1d+AmZIycsCh9odrqjObJHJa8/sEEUrM21KP64bF2\r\n"
"2JDBYbRmUjaiJlOqq3ReB30Zgtsq2B+g2Q0cLUlm91slc0boC4pPaQy1AJDh2oIQ\r\n"
"Zn2uVCuLZXmRoeJhw81ASQjuaAzxi4bSRr/QuKoRAx5/VqgaHkQYDw+Fi9qLRF7i\r\n"
"GMZiL8dmjfpd2H3zJ4kpAcWQDj8n8TDISg7v1t7HxydrxwU9esQCPJodPg/oNJhb\r\n"
"y3NLUpbYEaIsgIhpOVrTD7DeWS8Rx/fqEgEwlwIDAQABAoIBAQCXR0S8EIHFGORZ\r\n"
"++AtOg6eENxD+xVs0f1IeGz57Tjo3QnXX7VBZNdj+p1ECvhCE/G7XnkgU5hLZX+G\r\n"
"Z0jkz/tqJOI0vRSdLBbipHnWouyBQ4e/A1yIJdlBtqXxJ1KE/ituHRbNc4j4kL8Z\r\n"
"/r6pvwnTI0PSx2Eqs048YdS92LT6qAv4flbNDxMn2uY7s4ycS4Q8w1JXnCeaAnYm\r\n"
"WYI5wxO+bvRELR2Mcz5DmVnL8jRyml6l6582bSv5oufReFIbyPZbQWlXgYnpu6He\r\n"
"GTc7E1zKYQGG/9+DQUl/1vQuCPqQwny0tQoX2w5tdYpdMdVm+zkLtbajzdTviJJa\r\n"
"TWzL6lt5AoGBAN86+SVeJDcmQJcv4Eq6UhtRr4QGMiQMz0Sod6ettYxYzMgxtw28\r\n"
"CIrgpozCc+UaZJLo7UxvC6an85r1b2nKPCLQFaggJ0H4Q0J/sZOhBIXaoBzWxveK\r\n"
"nupceKdVxGsFi8CDy86DBfiyFivfBj+47BbaQzPBj7C4rK7UlLjab2rDAoGBAN2u\r\n"
"AM2gchoFiu4v1HFL8D7lweEpi6ZnMJjnEu/dEgGQJFjwdpLnPbsj4c75odQ4Gz8g\r\n"
"sw9lao9VVzbusoRE/JGI4aTdO0pATXyG7eG1Qu+5Yc1YGXcCrliA2xM9xx+d7f+s\r\n"
"mPzN+WIEg5GJDYZDjAzHG5BNvi/FfM1C9dOtjv2dAoGAF0t5KmwbjWHBhcVqO4Ic\r\n"
"BVvN3BIlc1ue2YRXEDlxY5b0r8N4XceMgKmW18OHApZxfl8uPDauWZLXOgl4uepv\r\n"
"whZC3EuWrSyyICNhLY21Ah7hbIEBPF3L3ZsOwC+UErL+dXWLdB56Jgy3gZaBeW7b\r\n"
"vDrEnocJbqCm7IukhXHOBK8CgYEAwqdHB0hqyNSzIOGY7v9abzB6pUdA3BZiQvEs\r\n"
"3LjHVd4HPJ2x0N8CgrBIWOE0q8+0hSMmeE96WW/7jD3fPWwCR5zlXknxBQsfv0gP\r\n"
"3BC5PR0Qdypz+d+9zfMf625kyit4T/hzwhDveZUzHnk1Cf+IG7Q+TOEnLnWAWBED\r\n"
"ISOWmrUCgYAFEmRxgwAc/u+D6t0syCwAYh6POtscq9Y0i9GyWk89NzgC4NdwwbBH\r\n"
"4AgahOxIxXx2gxJnq3yfkJfIjwf0s2DyP0kY2y6Ua1OeomPeY9mrIS4tCuDQ6LrE\r\n"
"TB6l9VGoxJL4fyHnZb8L5gGvnB1bbD8cL6YPaDiOhcRseC9vBiEuVg==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

const char mbedtls_test_cli_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDPzCCAiegAwIBAgIBBDANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"
"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
"MTEwMjEyMTQ0NDA3WhcNMjEwMjEyMTQ0NDA3WjA8MQswCQYDVQQGEwJOTDERMA8G\r\n"
"A1UEChMIUG9sYXJTU0wxGjAYBgNVBAMTEVBvbGFyU1NMIENsaWVudCAyMIIBIjAN\r\n"
"BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6f\r\n"
"M60Nj4o8VmXl3ETZzGaFB9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu\r\n"
"1C93KYRhTYJQj6eVSHD1bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEw\r\n"
"MjDV0/YI0FZPRo7yX/k9Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v\r\n"
"4Jv4EFbMs44TFeY0BGbH7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx/\r\n"
"/DZrtenNLQNiTrM9AM+vdqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQAB\r\n"
"o00wSzAJBgNVHRMEAjAAMB0GA1UdDgQWBBRxoQBzckAvVHZeM/xSj7zx3WtGITAf\r\n"
"BgNVHSMEGDAWgBS0WuSls97SUva51aaVD+s+vMf9/zANBgkqhkiG9w0BAQUFAAOC\r\n"
"AQEAAn86isAM8X+mVwJqeItt6E9slhEQbAofyk+diH1Lh8Y9iLlWQSKbw/UXYjx5\r\n"
"LLPZcniovxIcARC/BjyZR9g3UwTHNGNm+rwrqa15viuNOFBchykX/Orsk02EH7NR\r\n"
"Alw5WLPorYjED6cdVQgBl9ot93HdJogRiXCxErM7NC8/eP511mjq+uLDjLKH8ZPQ\r\n"
"8I4ekHJnroLsDkIwXKGIsvIBHQy2ac/NwHLCQOK6mfum1pRx52V4Utu5dLLjD5bM\r\n"
"xOBC7KU4xZKuMXXZM6/93Yb51K/J4ahf1TxJlTWXtnzDr9saEYdNy2SKY/6ZiDNH\r\n"
"D+stpAKiQLAWaAusIWKYEyw9MQ==\r\n"
"-----END CERTIFICATE-----\r\n";

const char mbedtls_test_cli_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6fM60Nj4o8VmXl3ETZzGaF\r\n"
"B9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu1C93KYRhTYJQj6eVSHD1\r\n"
"bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEwMjDV0/YI0FZPRo7yX/k9\r\n"
"Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v4Jv4EFbMs44TFeY0BGbH\r\n"
"7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx//DZrtenNLQNiTrM9AM+v\r\n"
"dqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQABAoIBAGdNtfYDiap6bzst\r\n"
"yhCiI8m9TtrhZw4MisaEaN/ll3XSjaOG2dvV6xMZCMV+5TeXDHOAZnY18Yi18vzz\r\n"
"4Ut2TnNFzizCECYNaA2fST3WgInnxUkV3YXAyP6CNxJaCmv2aA0yFr2kFVSeaKGt\r\n"
"ymvljNp2NVkvm7Th8fBQBO7I7AXhz43k0mR7XmPgewe8ApZOG3hstkOaMvbWAvWA\r\n"
"zCZupdDjZYjOJqlA4eEA4H8/w7F83r5CugeBE8LgEREjLPiyejrU5H1fubEY+h0d\r\n"
"l5HZBJ68ybTXfQ5U9o/QKA3dd0toBEhhdRUDGzWtjvwkEQfqF1reGWj/tod/gCpf\r\n"
"DFi6X0ECgYEA4wOv/pjSC3ty6TuOvKX2rOUiBrLXXv2JSxZnMoMiWI5ipLQt+RYT\r\n"
"VPafL/m7Dn6MbwjayOkcZhBwk5CNz5A6Q4lJ64Mq/lqHznRCQQ2Mc1G8eyDF/fYL\r\n"
"Ze2pLvwP9VD5jTc2miDfw+MnvJhywRRLcemDFP8k4hQVtm8PMp3ZmNECgYEA4gz7\r\n"
"wzObR4gn8ibe617uQPZjWzUj9dUHYd+in1gwBCIrtNnaRn9I9U/Q6tegRYpii4ys\r\n"
"c176NmU+umy6XmuSKV5qD9bSpZWG2nLFnslrN15Lm3fhZxoeMNhBaEDTnLT26yoi\r\n"
"33gp0mSSWy94ZEqipms+ULF6sY1ZtFW6tpGFoy8CgYAQHhnnvJflIs2ky4q10B60\r\n"
"ZcxFp3rtDpkp0JxhFLhiizFrujMtZSjYNm5U7KkgPVHhLELEUvCmOnKTt4ap/vZ0\r\n"
"BxJNe1GZH3pW6SAvGDQpl9sG7uu/vTFP+lCxukmzxB0DrrDcvorEkKMom7ZCCRvW\r\n"
"KZsZ6YeH2Z81BauRj218kQKBgQCUV/DgKP2985xDTT79N08jUo3hTP5MVYCCuj/+\r\n"
"UeEw1TvZcx3LJby7P6Xad6a1/BqveaGyFKIfEFIaBUBItk801sDDpDaYc4gL00Xc\r\n"
"7lFuBHOZkxJYlss5QrGpuOEl9ZwUt5IrFLBdYaKqNHzNVC1pCPfb/JyH6Dr2HUxq\r\n"
"gxUwAQKBgQCcU6G2L8AG9d9c0UpOyL1tMvFe5Ttw0KjlQVdsh1MP6yigYo9DYuwu\r\n"
"bHFVW2r0dBTqegP2/KTOxKzaHfC1qf0RGDsUoJCNJrd1cwoCLG8P2EF4w3OBrKqv\r\n"
"8u4ytY0F+Vlanj5lm3TaoHSVF1+NWPyOTiwevIECGKwSxvlki4fDAA==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

const size_t mbedtls_test_ca_crt_rsa_len  = sizeof( mbedtls_test_ca_crt_rsa );
const size_t mbedtls_test_ca_key_rsa_len  = sizeof( mbedtls_test_ca_key_rsa );
const size_t mbedtls_test_ca_pwd_rsa_len  = sizeof( mbedtls_test_ca_pwd_rsa ) - 1;
const size_t mbedtls_test_srv_crt_rsa_len = sizeof( mbedtls_test_srv_crt_rsa );
const size_t mbedtls_test_srv_key_rsa_len = sizeof( mbedtls_test_srv_key_rsa );
const size_t mbedtls_test_cli_crt_rsa_len = sizeof( mbedtls_test_cli_crt_rsa );
const size_t mbedtls_test_cli_key_rsa_len = sizeof( mbedtls_test_cli_key_rsa );
#else
#define TEST_CA_CRT_RSA
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_PEM_PARSE_C)
/* Concatenation of all available CA certificates */
const char mbedtls_test_cas_pem[] = TEST_CA_CRT_RSA TEST_CA_CRT_EC;
const size_t mbedtls_test_cas_pem_len = sizeof( mbedtls_test_cas_pem );
#endif

/* List of all available CA certificates */
const char * mbedtls_test_cas[] = {
#if defined(MBEDTLS_RSA_C)
    mbedtls_test_ca_crt_rsa,
#endif
#if defined(MBEDTLS_ECDSA_C)
    mbedtls_test_ca_crt_ec,
#endif
    NULL
};
const size_t mbedtls_test_cas_len[] = {
#if defined(MBEDTLS_RSA_C)
    sizeof( mbedtls_test_ca_crt_rsa ),
#endif
#if defined(MBEDTLS_ECDSA_C)
    sizeof( mbedtls_test_ca_crt_ec ),
#endif
    0
};

#if defined(MBEDTLS_RSA_C)
const char *mbedtls_test_ca_crt  = mbedtls_test_ca_crt_rsa;
const char *mbedtls_test_ca_key  = mbedtls_test_ca_key_rsa;
const char *mbedtls_test_ca_pwd  = mbedtls_test_ca_pwd_rsa;
const char *mbedtls_test_srv_crt = mbedtls_test_srv_crt_rsa;
const char *mbedtls_test_srv_key = mbedtls_test_srv_key_rsa;
const char *mbedtls_test_cli_crt = mbedtls_test_cli_crt_rsa;
const char *mbedtls_test_cli_key = mbedtls_test_cli_key_rsa;
const size_t mbedtls_test_ca_crt_len  = sizeof( mbedtls_test_ca_crt_rsa );
const size_t mbedtls_test_ca_key_len  = sizeof( mbedtls_test_ca_key_rsa );
const size_t mbedtls_test_ca_pwd_len  = sizeof( mbedtls_test_ca_pwd_rsa ) - 1;
const size_t mbedtls_test_srv_crt_len = sizeof( mbedtls_test_srv_crt_rsa );
const size_t mbedtls_test_srv_key_len = sizeof( mbedtls_test_srv_key_rsa );
const size_t mbedtls_test_cli_crt_len = sizeof( mbedtls_test_cli_crt_rsa );
const size_t mbedtls_test_cli_key_len = sizeof( mbedtls_test_cli_key_rsa );
#else /* ! MBEDTLS_RSA_C, so MBEDTLS_ECDSA_C */
const char *mbedtls_test_ca_crt  = mbedtls_test_ca_crt_ec;
const char *mbedtls_test_ca_key  = mbedtls_test_ca_key_ec;
const char *mbedtls_test_ca_pwd  = mbedtls_test_ca_pwd_ec;
const char *mbedtls_test_srv_crt = mbedtls_test_srv_crt_ec;
const char *mbedtls_test_srv_key = mbedtls_test_srv_key_ec;
const char *mbedtls_test_cli_crt = mbedtls_test_cli_crt_ec;
const char *mbedtls_test_cli_key = mbedtls_test_cli_key_ec;
const size_t mbedtls_test_ca_crt_len  = sizeof( mbedtls_test_ca_crt_ec );
const size_t mbedtls_test_ca_key_len  = sizeof( mbedtls_test_ca_key_ec );
const size_t mbedtls_test_ca_pwd_len  = sizeof( mbedtls_test_ca_pwd_ec ) - 1;
const size_t mbedtls_test_srv_crt_len = sizeof( mbedtls_test_srv_crt_ec );
const size_t mbedtls_test_srv_key_len = sizeof( mbedtls_test_srv_key_ec );
const size_t mbedtls_test_cli_crt_len = sizeof( mbedtls_test_cli_crt_ec );
const size_t mbedtls_test_cli_key_len = sizeof( mbedtls_test_cli_key_ec );
#endif /* MBEDTLS_RSA_C */

#endif /* MBEDTLS_CERTS_C */
