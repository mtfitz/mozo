if(DEFINED OpenSSL_FOUND)
    set(OPENSSL_FOUND ${OpenSSL_FOUND})
endif()
if(DEFINED OpenSSL_INCLUDE_DIR)
    set(OPENSSL_INCLUDE_DIR ${OpenSSL_INCLUDE_DIR})
endif()
if(DEFINED OpenSSL_Crypto_LIBS)
    set(OPENSSL_CRYPTO_LIBRARY ${OpenSSL_Crypto_LIBS})
    set(OPENSSL_CRYPTO_LIBRARIES ${OpenSSL_Crypto_LIBS}
                                 ${OpenSSL_Crypto_DEPENDENCIES}
                                 ${OpenSSL_Crypto_FRAMEWORKS}
                                 ${OpenSSL_Crypto_SYSTEM_LIBS})
endif()
if(DEFINED OpenSSL_SSL_LIBS)
    set(OPENSSL_SSL_LIBRARY ${OpenSSL_SSL_LIBS})
    set(OPENSSL_SSL_LIBRARIES ${OpenSSL_SSL_LIBS}
                              ${OpenSSL_SSL_DEPENDENCIES}
                              ${OpenSSL_SSL_FRAMEWORKS}
                              ${OpenSSL_SSL_SYSTEM_LIBS})
endif()
if(DEFINED OpenSSL_LIBRARIES)
    set(OPENSSL_LIBRARIES ${OpenSSL_LIBRARIES})
endif()
if(DEFINED OpenSSL_VERSION)
    set(OPENSSL_VERSION ${OpenSSL_VERSION})
endif()
