//
// Created by swift on 2020/3/3.
//

#pragma once

#include <base/marcos.h>
#include <map>

namespace Code {

#define MAX_BUFFER UINT16_MAX
#define MAX_BLOCK_BIT 26

    struct BlockHeader {
        char magic_[4];
        u32 arch_;
        u32 version_;
        u32 timestamp_;
        u16 block_count_;
        u32 size_;
    };

    struct Buffer {
        u16 id_;
        struct {
            u32 offset_:24;
            u32 version_:8;
        };
        // 4 *
        u16 size_;
        VAddr source_;
    };

    class BaseBlock {
    public:

        BaseBlock(VAddr start, VAddr size);

        VAddr GetBufferStart(Buffer &buffer);
        VAddr GetBufferEnd(Buffer &buffer);

        Buffer &GetBuffer(u16 id);

        virtual Buffer &AllocCodeBuffer(VAddr source, u32 size);
        virtual bool SaveToDisk(std::string path);
    protected:
        VAddr start_;
        VAddr size_;
        std::mutex lock_;
        u16 current_buffer_id_{0};
        u32 current_offset_{0};
        std::array<Buffer, MAX_BUFFER> buffers_;
        std::map<VAddr, u16> buffers_map_;
    };

    namespace A64 {

        /**
         * 考虑到指令的可修改性
         * 我们需要一个中间人来分发代码
         * 当指令被修改时，我们生成新的指令，并且将分发表指向新指令缓存
         * 直接修改旧缓存，当有其他线程还在执行时会引起不必要的错误
         */

        struct Dispatcher {
            // B label
            u32 instr_direct_branch_;
        };

        struct DispatcherTable {
            Dispatcher *dispatchers_;
        };

#define BLOCK_SIZE_A64 16 * 1024 * 1024

        class CodeBlock : public BaseBlock {
        public:
            CodeBlock(u32 block_size = BLOCK_SIZE_A64);
            virtual ~CodeBlock();

            void GenDispatcher(Buffer &buffer);

        protected:
            u32 dispatcher_count_;
            DispatcherTable *dispatcher_table_;
        };

    }

}
