# ec_protobuf
another google protocol buffers packaging and unpacking C++ class, single h file is less than 800 lines.

google protocol buffers的编码解码规则很简单，参见 https://developers.google.com/protocol-buffers/docs/encoding 相对于google提供的工程工具生成的代码，我更喜欢小巧的手工打造的代码。
这里提供的编码解码类基于C++11，支持proto3规范，并支持简单类型紧凑数组比如 repeated int32 data = 16 [packed = true] 这样的编码/解码。在多个工程应用过，和google工具生成的代码双向兼容。

整个代码加上注释不到800行，使用时只需include "ec_protobuf.h"即可，ezpb.cpp中演示了如何使用。
