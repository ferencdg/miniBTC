file(GLOB SIGN_TRANSACTION_SOURCES "SignTransaction.cpp")
add_executable(sign_transaction ${SIGN_TRANSACTION_SOURCES})
target_link_libraries (sign_transaction LINK_PUBLIC ${EXTERNAL_LIBS})
