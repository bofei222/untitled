// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <cerrno>
#include <deque>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "arrow/array.h"
#include "arrow/c/bridge.h"
#include "arrow/c/helpers.h"
#include "arrow/c/util_internal.h"
#include <arrow/api.h>

#include "arrow/ipc/json_simple.h"
#include "arrow/memory_pool.h"
#include "arrow/testing/builder.h"
#include "arrow/testing/extension_type.h"
#include "arrow/testing/gtest_util.h"
#include "arrow/testing/matchers.h"
#include "arrow/testing/util.h"
#include "arrow/util/async_generator.h"
#include "arrow/util/binary_view_util.h"
#include "arrow/util/checked_cast.h"
#include "arrow/util/endian.h"
#include "arrow/util/key_value_metadata.h"
#include "arrow/util/logging.h"
#include "arrow/util/macros.h"
#include "arrow/util/range.h"
#include <arrow/c/bridge.h>
#include "arrow/util/thread_pool.h"

// TODO(GH-37221): Remove these ifdef checks when compute dependency is removed
#ifdef ARROW_COMPUTE
#  include "arrow/compute/api_vector.h"
#endif

namespace arrow {
    using internal::ArrayDeviceExportTraits;
    using internal::ArrayDeviceStreamExportTraits;
    using internal::ArrayExportGuard;
    using internal::ArrayExportTraits;
    using internal::ArrayStreamExportGuard;
    using internal::ArrayStreamExportTraits;
    using internal::checked_cast;
    using internal::DeviceArrayExportGuard;
    using internal::DeviceArrayStreamExportGuard;
    using internal::SchemaExportGuard;
    using internal::SchemaExportTraits;
    using internal::Zip;

    template<typename T>
    struct ExportTraits {
    };

    template<typename T>
    using Exporter = std::function<Status(const T &, struct ArrowSchema *)>;

    template<>
    struct ExportTraits<DataType> {
        static Exporter<DataType> ExportFunc;
    };

    template<>
    struct ExportTraits<Field> {
        static Exporter<Field> ExportFunc;
    };

    template<>
    struct ExportTraits<Schema> {
        static Exporter<Schema> ExportFunc;
    };

    Exporter<DataType> ExportTraits<DataType>::ExportFunc = ExportType;
    Exporter<Field> ExportTraits<Field>::ExportFunc = ExportField;
    Exporter<Schema> ExportTraits<Schema>::ExportFunc = ExportSchema;

    // An interceptor that checks whether a release callback was called.
    // (for import tests)
    template<typename Traits>
    class ReleaseCallback {
    public:
        using CType = typename Traits::CType;

        explicit ReleaseCallback(CType *c_struct) {
            orig_release_ = c_struct->release;
            orig_private_data_ = c_struct->private_data;
            c_struct->release = StaticRelease;
            c_struct->private_data = this;
        }

        static void StaticRelease(CType *c_struct) {
            reinterpret_cast<ReleaseCallback *>(c_struct->private_data)->Release(c_struct);
        }

        void Release(CType *c_struct) {
            ASSERT_FALSE(called_) << "ReleaseCallback called twice";
            called_ = true;
            ASSERT_FALSE(Traits::IsReleasedFunc(c_struct))
          << "ReleaseCallback called with released Arrow"
          << (std::is_same<CType, ArrowSchema>::value ? "Schema" : "Array");
            // Call original release callback
            c_struct->release = orig_release_;
            c_struct->private_data = orig_private_data_;
            Traits::ReleaseFunc(c_struct);
            ASSERT_TRUE(Traits::IsReleasedFunc(c_struct))
          << "ReleaseCallback did not release ArrowSchema";
        }

        void AssertCalled() {
            ASSERT_TRUE(called_) << "ReleaseCallback was not called";
        }

        void AssertNotCalled() {
            ASSERT_FALSE(called_) << "ReleaseCallback was called";
        }

    private:
        ARROW_DISALLOW_COPY_AND_ASSIGN(ReleaseCallback);

        bool called_{false};

        void (*orig_release_)(CType *);

        void *orig_private_data_;
    };

    using SchemaReleaseCallback = ReleaseCallback<SchemaExportTraits>;
    using ArrayReleaseCallback = ReleaseCallback<ArrayExportTraits>;
    constexpr std::string_view binary_view_buffer_content0 = "12345foo bar baz quux",
            binary_view_buffer_content1 = "BinaryViewMultipleBuffers";

    // Whether c_struct or any of its descendents have non-null data pointers.
    bool HasData(const ArrowArray *c_struct) {
        for (int64_t i = 0; i < c_struct->n_buffers; ++i) {
            if (c_struct->buffers[i] != nullptr) {
                return true;
            }
        }
        if (c_struct->dictionary && HasData(c_struct->dictionary)) {
            return true;
        }
        for (int64_t i = 0; i < c_struct->n_children; ++i) {
            if (HasData(c_struct->children[i])) {
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Data import tests

    // [true, false, true, true, false, true, true, true] * 2
    static const uint8_t bits_buffer1[] = {0xed, 0xed};

    static const void *buffers_no_nulls_no_data[1] = {nullptr};
    static const void *buffers_nulls_no_data1[1] = {bits_buffer1};

    static const void *all_buffers_omitted[3] = {nullptr, nullptr, nullptr};

    static const uint8_t data_buffer1[] = {
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16
    };
    static const uint8_t data_buffer2[] = "abcdefghijklmnopqrstuvwxyz";
#if ARROW_LITTLE_ENDIAN
    static const uint64_t data_buffer3[] = {123456789, 0, 987654321, 0};
#else
static const uint64_t data_buffer3[] = {0, 123456789, 0, 987654321};
#endif
    static const uint8_t data_buffer4[] = {1, 2, 0, 1, 3, 0};
    static const float data_buffer5[] = {0.0f, 1.5f, -2.0f, 3.0f, 4.0f, 5.0f};
    static const double data_buffer6[] = {0.0, 1.5, -2.0, 3.0, 4.0, 5.0};
    static const int32_t data_buffer7[] = {1234, 5678, 9012, 3456};
    static const int64_t data_buffer8[] = {123456789, 987654321, -123456789, -987654321};
    static const int64_t date64_data_buffer8[] = {86400000, 172800000, -86400000, -172800000};
#if ARROW_LITTLE_ENDIAN
    static const void *primitive_buffers_no_nulls1_8[2] = {nullptr, data_buffer1};
    static const void *primitive_buffers_no_nulls1_16[2] = {nullptr, data_buffer1};
    static const void *primitive_buffers_no_nulls1_32[2] = {nullptr, data_buffer1};
    static const void *primitive_buffers_no_nulls1_64[2] = {nullptr, data_buffer1};
    static const void *primitive_buffers_nulls1_8[2] = {bits_buffer1, data_buffer1};
    static const void *primitive_buffers_nulls1_16[2] = {bits_buffer1, data_buffer1};
#else
static const uint8_t data_buffer1_16[] = {2,  1, 4,  3,  6,  5,  8,  7,
                                          10, 9, 12, 11, 14, 13, 16, 15};
static const uint8_t data_buffer1_32[] = {4,  3,  2,  1, 8,  7,  6,  5,
                                          12, 11, 10, 9, 16, 15, 14, 13};
static const uint8_t data_buffer1_64[] = {8,  7,  6,  5,  4,  3,  2,  1,
                                          16, 15, 14, 13, 12, 11, 10, 9};
static const void* primitive_buffers_no_nulls1_8[2] = {nullptr, data_buffer1};
static const void* primitive_buffers_no_nulls1_16[2] = {nullptr, data_buffer1_16};
static const void* primitive_buffers_no_nulls1_32[2] = {nullptr, data_buffer1_32};
static const void* primitive_buffers_no_nulls1_64[2] = {nullptr, data_buffer1_64};
static const void* primitive_buffers_nulls1_8[2] = {bits_buffer1, data_buffer1};
static const void* primitive_buffers_nulls1_16[2] = {bits_buffer1, data_buffer1_16};
#endif
    static const void *primitive_buffers_no_nulls2[2] = {nullptr, data_buffer2};
    static const void *primitive_buffers_no_nulls3[2] = {nullptr, data_buffer3};
    static const void *primitive_buffers_no_nulls4[2] = {nullptr, data_buffer4};
    static const void *primitive_buffers_no_nulls5[2] = {nullptr, data_buffer5};
    static const void *primitive_buffers_no_nulls6[2] = {nullptr, data_buffer6};
    static const void *primitive_buffers_no_nulls7[2] = {nullptr, data_buffer7};
    static const void *primitive_buffers_nulls7[2] = {bits_buffer1, data_buffer7};
    static const void *primitive_buffers_no_nulls8[2] = {nullptr, data_buffer8};
    static const void *primitive_buffers_nulls8[2] = {bits_buffer1, data_buffer8};

    static const void *date64_buffers_no_nulls8[2] = {nullptr, date64_data_buffer8};
    static const void *date64_buffers_nulls8[2] = {bits_buffer1, date64_data_buffer8};

    static const int64_t timestamp_data_buffer1[] = {0, 951782400, -2203977600LL};
    static const int64_t timestamp_data_buffer2[] = {0, 951782400000LL, -2203977600000LL};
    static const int64_t timestamp_data_buffer3[] = {
        0, 951782400000000LL,
        -2203977600000000LL
    };
    static const int64_t timestamp_data_buffer4[] = {
        0, 951782400000000000LL,
        -2203977600000000000LL
    };
    static const void *timestamp_buffers_no_nulls1[2] = {nullptr, timestamp_data_buffer1};
    static const void *timestamp_buffers_nulls1[2] = {bits_buffer1, timestamp_data_buffer1};
    static const void *timestamp_buffers_no_nulls2[2] = {nullptr, timestamp_data_buffer2};
    static const void *timestamp_buffers_no_nulls3[2] = {nullptr, timestamp_data_buffer3};
    static const void *timestamp_buffers_no_nulls4[2] = {nullptr, timestamp_data_buffer4};

    static const uint16_t run_ends_data_buffer5[5] = {1, 2, 4, 7, 9};
    [[maybe_unused]] static const void *run_ends_buffers5[2] = {
        nullptr,
        run_ends_data_buffer5
    };

    static const uint8_t string_data_buffer1[] = "foobarquuxxyzzy";

    static const int32_t string_offsets_buffer1[] = {0, 3, 3, 6, 10, 15};
    static const void *string_buffers_no_nulls1[3] = {
        nullptr, string_offsets_buffer1,
        string_data_buffer1
    };
    static const void *string_buffers_omitted[3] = {nullptr, string_offsets_buffer1, nullptr};

    static const int64_t large_string_offsets_buffer1[] = {0, 3, 3, 6, 10};
    static const void *large_string_buffers_no_nulls1[3] = {
        nullptr, large_string_offsets_buffer1, string_data_buffer1
    };
    static const void *large_string_buffers_omitted[3] = {
        nullptr, large_string_offsets_buffer1, nullptr
    };

    constexpr int64_t binary_view_buffer_sizes1[] = {
        binary_view_buffer_content0.size(),
        binary_view_buffer_content1.size()
    };

    static const BinaryViewType::c_type binary_view_buffer1[] = {
        util::ToBinaryView(binary_view_buffer_content0, 0, 0),
        util::ToInlineBinaryView("foo"),
        util::ToBinaryView(binary_view_buffer_content1, 1, 0),
        util::ToInlineBinaryView("bar"),
        util::ToBinaryView(binary_view_buffer_content0.substr(5), 0, 5),
        util::ToInlineBinaryView("baz"),
        util::ToBinaryView(binary_view_buffer_content1.substr(6, 13), 1, 6),
        util::ToInlineBinaryView("quux"),
    };
    static const void *binary_view_buffers_no_nulls1[] = {
        nullptr,
        binary_view_buffer1,
        binary_view_buffer_content0.data(),
        binary_view_buffer_content1.data(),
        binary_view_buffer_sizes1,
    };

    static const int32_t list_offsets_buffer1[] = {0, 2, 2, 5, 6, 8};
    static const void *list_buffers_no_nulls1[2] = {nullptr, list_offsets_buffer1};
    static const void *list_buffers_nulls1[2] = {bits_buffer1, list_offsets_buffer1};

    static const int64_t large_list_offsets_buffer1[] = {0, 2, 2, 5, 6, 8};
    static const void *large_list_buffers_no_nulls1[2] = {
        nullptr,
        large_list_offsets_buffer1
    };

    static const int32_t list_view_offsets_buffer1[] = {0, 2, 2, 5, 6};
    static const int32_t list_view_sizes_buffer1[] = {2, 0, 3, 1, 2};
    static const void *list_view_buffers_no_nulls1[3] = {
        nullptr, list_view_offsets_buffer1,
        list_view_sizes_buffer1
    };
    static const void *list_view_buffers_nulls1[3] = {
        bits_buffer1, list_view_offsets_buffer1,
        list_view_sizes_buffer1
    };

    static const int64_t large_list_view_offsets_buffer1[] = {0, 2, 2, 5, 6};
    static const int64_t large_list_view_sizes_buffer1[] = {2, 0, 3, 1, 2};
    static const void *large_list_view_buffers_no_nulls1[3] = {
        nullptr, large_list_view_offsets_buffer1, large_list_view_sizes_buffer1
    };

    static const int8_t type_codes_buffer1[] = {42, 42, 43, 43, 42};
    static const int32_t union_offsets_buffer1[] = {0, 1, 0, 1, 2};
    static const void *sparse_union_buffers1_legacy[2] = {nullptr, type_codes_buffer1};
    static const void *dense_union_buffers1_legacy[3] = {
        nullptr, type_codes_buffer1,
        union_offsets_buffer1
    };
    static const void *sparse_union_buffers1[1] = {type_codes_buffer1};
    static const void *dense_union_buffers1[2] = {type_codes_buffer1, union_offsets_buffer1};

    void NoOpArrayRelease(struct ArrowArray *schema) { ArrowArrayMarkReleased(schema); }

    class TestArrayImport : public ::testing::Test {
    public:
        void SetUp() override { Reset(); }

        void Reset() {
            memset(&c_struct_, 0, sizeof(c_struct_));
            c_struct_.release = NoOpArrayRelease;
            nested_structs_.clear();
            children_arrays_.clear();
        }

        // Create a new ArrowArray struct with a stable C pointer
        struct ArrowArray *AddChild() {
            nested_structs_.emplace_back();
            struct ArrowArray *result = &nested_structs_.back();
            memset(result, 0, sizeof(*result));
            result->release = NoOpArrayRelease;
            return result;
        }

        // Create a stable C pointer to the N last structs in nested_structs_
        struct ArrowArray **NLastChildren(int64_t n_children, struct ArrowArray *parent) {
            children_arrays_.emplace_back(n_children);
            struct ArrowArray **children = children_arrays_.back().data();
            int64_t nested_offset;
            // If parent is itself at the end of nested_structs_, skip it
            if (parent != nullptr && &nested_structs_.back() == parent) {
                nested_offset = static_cast<int64_t>(nested_structs_.size()) - n_children - 1;
            } else {
                nested_offset = static_cast<int64_t>(nested_structs_.size()) - n_children;
            }
            for (int64_t i = 0; i < n_children; ++i) {
                children[i] = &nested_structs_[nested_offset + i];
            }
            return children;
        }

        struct ArrowArray *LastChild(struct ArrowArray *parent = nullptr) {
            return *NLastChildren(1, parent);
        }

        void FillPrimitive(struct ArrowArray *c, int64_t length, int64_t null_count,
                           int64_t offset, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 2;
            c->buffers = buffers;
        }

        void FillDictionary(struct ArrowArray *c) { c->dictionary = LastChild(c); }

        void FillStringLike(struct ArrowArray *c, int64_t length, int64_t null_count,
                            int64_t offset, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 3;
            c->buffers = buffers;
        }

        void FillStringViewLike(struct ArrowArray *c, int64_t length, int64_t null_count,
                                int64_t offset, const void **buffers,
                                int32_t data_buffer_count) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 2 + data_buffer_count + 1;
            c->buffers = buffers;
        }

        void FillListLike(struct ArrowArray *c, int64_t length, int64_t null_count,
                          int64_t offset, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 2;
            c->buffers = buffers;
            c->n_children = 1;
            c->children = NLastChildren(1, c);
        }

        void FillListView(struct ArrowArray *c, int64_t length, int64_t null_count,
                          int64_t offset, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 3;
            c->buffers = buffers;
            c->n_children = 1;
            c->children = NLastChildren(1, c);
        }

        void FillFixedSizeListLike(struct ArrowArray *c, int64_t length, int64_t null_count,
                                   int64_t offset, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 1;
            c->buffers = buffers;
            c->n_children = 1;
            c->children = NLastChildren(1, c);
        }

        void FillStructLike(struct ArrowArray *c, int64_t length, int64_t null_count,
                            int64_t offset, int64_t n_children, const void **buffers) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            c->n_buffers = 1;
            c->buffers = buffers;
            c->n_children = n_children;
            c->children = NLastChildren(c->n_children, c);
        }

        // `legacy` selects pre-ARROW-14179 behaviour
        void FillUnionLike(struct ArrowArray *c, UnionMode::type mode, int64_t length,
                           int64_t null_count, int64_t offset, int64_t n_children,
                           const void **buffers, bool legacy) {
            c->length = length;
            c->null_count = null_count;
            c->offset = offset;
            if (mode == UnionMode::SPARSE) {
                c->n_buffers = legacy ? 2 : 1;
            } else {
                c->n_buffers = legacy ? 3 : 2;
            }
            c->buffers = buffers;
            c->n_children = n_children;
            c->children = NLastChildren(c->n_children, c);
        }

        void FillPrimitive(int64_t length, int64_t null_count, int64_t offset,
                           const void **buffers) {
            FillPrimitive(&c_struct_, length, null_count, offset, buffers);
        }

        void FillDictionary() { FillDictionary(&c_struct_); }

        void FillStringLike(int64_t length, int64_t null_count, int64_t offset,
                            const void **buffers) {
            FillStringLike(&c_struct_, length, null_count, offset, buffers);
        }

        void FillStringViewLike(int64_t length, int64_t null_count, int64_t offset,
                                const void **buffers, int32_t data_buffer_count) {
            FillStringViewLike(&c_struct_, length, null_count, offset, buffers,
                               data_buffer_count);
        }

        void FillListLike(int64_t length, int64_t null_count, int64_t offset,
                          const void **buffers) {
            FillListLike(&c_struct_, length, null_count, offset, buffers);
        }

        void FillListView(int64_t length, int64_t null_count, int64_t offset,
                          const void **buffers) {
            FillListView(&c_struct_, length, null_count, offset, buffers);
        }

        void FillFixedSizeListLike(int64_t length, int64_t null_count, int64_t offset,
                                   const void **buffers) {
            FillFixedSizeListLike(&c_struct_, length, null_count, offset, buffers);
        }

        void FillStructLike(int64_t length, int64_t null_count, int64_t offset,
                            int64_t n_children, const void **buffers) {
            FillStructLike(&c_struct_, length, null_count, offset, n_children, buffers);
        }

        void FillUnionLike(UnionMode::type mode, int64_t length, int64_t null_count,
                           int64_t offset, int64_t n_children, const void **buffers,
                           bool legacy) {
            FillUnionLike(&c_struct_, mode, length, null_count, offset, n_children, buffers,
                          legacy);
        }

        void FillRunEndEncoded(int64_t length, int64_t offset) {
            FillRunEndEncoded(&c_struct_, length, offset);
        }

        void FillRunEndEncoded(struct ArrowArray *c, int64_t length, int64_t offset) {
            c->length = length;
            c->null_count = 0;
            c->offset = offset;
            c->n_buffers = 0;
            c->buffers = nullptr;
            c->n_children = 2;
            c->children = NLastChildren(2, c);
        }

        void CheckImport(const std::shared_ptr<Array> &expected) {
            ArrayReleaseCallback cb(&c_struct_);

            auto type = expected->type();
            ASSERT_OK_AND_ASSIGN(auto array, ImportArray(&c_struct_, type));
            ASSERT_TRUE(ArrowArrayIsReleased(&c_struct_)); // was moved
            Reset(); // for further tests

            ASSERT_OK(array->ValidateFull());
            // Special case: arrays without data (such as Null arrays) needn't keep
            // the ArrowArray struct alive.
            if (HasData(&c_struct_)) {
                cb.AssertNotCalled();
            }
            AssertArraysEqual(*expected, *array, true);
            array.reset();
            cb.AssertCalled();
        }

        void CheckImport(const std::shared_ptr<RecordBatch> &expected) {
            ArrayReleaseCallback cb(&c_struct_);

            auto schema = expected->schema();
            ASSERT_OK_AND_ASSIGN(auto batch, ImportRecordBatch(&c_struct_, schema));
            ASSERT_TRUE(ArrowArrayIsReleased(&c_struct_)); // was moved
            Reset(); // for further tests

            ASSERT_OK(batch->ValidateFull());
            AssertBatchesEqual(*expected, *batch);
            cb.AssertNotCalled();
            batch.reset();
            cb.AssertCalled();
        }

        void CheckImportError(const std::shared_ptr<DataType> &type) {
            ArrayReleaseCallback cb(&c_struct_);

            ASSERT_RAISES(Invalid, ImportArray(&c_struct_, type));
            ASSERT_TRUE(ArrowArrayIsReleased(&c_struct_));
            Reset(); // for further tests
            cb.AssertCalled(); // was released
        }

        void CheckImportError(const std::shared_ptr<Schema> &schema) {
            ArrayReleaseCallback cb(&c_struct_);

            ASSERT_RAISES(Invalid, ImportRecordBatch(&c_struct_, schema));
            ASSERT_TRUE(ArrowArrayIsReleased(&c_struct_));
            Reset(); // for further tests
            cb.AssertCalled(); // was released
        }

    protected:
        struct ArrowArray c_struct_;
        // Deque elements don't move when the deque is appended to, which allows taking
        // stable C pointers to them.
        std::deque<struct ArrowArray> nested_structs_;
        std::deque<std::vector<struct ArrowArray *> > children_arrays_;
    };

    TEST_F(TestArrayImport, Primitive) {
        // Without nulls

        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls1_8);
        CheckImport(ArrayFromJSON(int8(), "[1, 2, 3]"));
        FillPrimitive(5, 0, 0, primitive_buffers_no_nulls1_8);
        CheckImport(ArrayFromJSON(uint8(), "[1, 2, 3, 4, 5]"));
        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls1_16);
        CheckImport(ArrayFromJSON(int16(), "[513, 1027, 1541]"));
        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls1_16);
        CheckImport(ArrayFromJSON(uint16(), "[513, 1027, 1541]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls1_32);
        CheckImport(ArrayFromJSON(int32(), "[67305985, 134678021]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls1_32);
        CheckImport(ArrayFromJSON(uint32(), "[67305985, 134678021]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls1_64);
        CheckImport(ArrayFromJSON(int64(), "[578437695752307201, 1157159078456920585]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls1_64);
        CheckImport(ArrayFromJSON(uint64(), "[578437695752307201, 1157159078456920585]"));

        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls1_8);
        CheckImport(ArrayFromJSON(boolean(), "[true, false, false]"));
        FillPrimitive(6, 0, 0, primitive_buffers_no_nulls5);
        CheckImport(ArrayFromJSON(float32(), "[0.0, 1.5, -2.0, 3.0, 4.0, 5.0]"));
        FillPrimitive(6, 0, 0, primitive_buffers_no_nulls6);
        CheckImport(ArrayFromJSON(float64(), "[0.0, 1.5, -2.0, 3.0, 4.0, 5.0]"));

        // With nulls
        FillPrimitive(9, -1, 0, primitive_buffers_nulls1_8);
        CheckImport(ArrayFromJSON(int8(), "[1, null, 3, 4, null, 6, 7, 8, 9]"));
        FillPrimitive(9, 2, 0, primitive_buffers_nulls1_8);
        CheckImport(ArrayFromJSON(int8(), "[1, null, 3, 4, null, 6, 7, 8, 9]"));
        FillPrimitive(3, -1, 0, primitive_buffers_nulls1_16);
        CheckImport(ArrayFromJSON(int16(), "[513, null, 1541]"));
        FillPrimitive(3, 1, 0, primitive_buffers_nulls1_16);
        CheckImport(ArrayFromJSON(int16(), "[513, null, 1541]"));
        FillPrimitive(3, -1, 0, primitive_buffers_nulls1_8);
        CheckImport(ArrayFromJSON(boolean(), "[true, null, false]"));
        FillPrimitive(3, 1, 0, primitive_buffers_nulls1_8);
        CheckImport(ArrayFromJSON(boolean(), "[true, null, false]"));

        // Empty array with null data pointers
        FillPrimitive(0, 0, 0, all_buffers_omitted);
        CheckImport(ArrayFromJSON(int32(), "[]"));
    }

    TEST_F(TestArrayImport, Temporal) {
        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls7);
        CheckImport(ArrayFromJSON(date32(), "[1234, 5678, 9012]"));
        FillPrimitive(3, 0, 0, date64_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(date64(), "[86400000, 172800000, -86400000]"));

        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls7);
        CheckImport(ArrayFromJSON(time32(TimeUnit::SECOND), "[1234, 5678]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls7);
        CheckImport(ArrayFromJSON(time32(TimeUnit::MILLI), "[1234, 5678]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(time64(TimeUnit::MICRO), "[123456789, 987654321]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(time64(TimeUnit::NANO), "[123456789, 987654321]"));

        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(duration(TimeUnit::SECOND), "[123456789, 987654321]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(duration(TimeUnit::MILLI), "[123456789, 987654321]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(duration(TimeUnit::MICRO), "[123456789, 987654321]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls8);
        CheckImport(ArrayFromJSON(duration(TimeUnit::NANO), "[123456789, 987654321]"));

        FillPrimitive(3, 0, 0, primitive_buffers_no_nulls7);
        CheckImport(ArrayFromJSON(month_interval(), "[1234, 5678, 9012]"));
        FillPrimitive(2, 0, 0, primitive_buffers_no_nulls7);
        CheckImport(ArrayFromJSON(day_time_interval(), "[[1234, 5678], [9012, 3456]]"));

        const char *json = R"(["1970-01-01","2000-02-29","1900-02-28"])";
        FillPrimitive(3, 0, 0, timestamp_buffers_no_nulls1);
        CheckImport(ArrayFromJSON(timestamp(TimeUnit::SECOND), json));
        FillPrimitive(3, 0, 0, timestamp_buffers_no_nulls2);
        CheckImport(ArrayFromJSON(timestamp(TimeUnit::MILLI), json));
        FillPrimitive(3, 0, 0, timestamp_buffers_no_nulls3);
        CheckImport(ArrayFromJSON(timestamp(TimeUnit::MICRO), json));
        FillPrimitive(3, 0, 0, timestamp_buffers_no_nulls4);
        CheckImport(ArrayFromJSON(timestamp(TimeUnit::NANO), json));

        // With nulls
        FillPrimitive(3, -1, 0, primitive_buffers_nulls7);
        CheckImport(ArrayFromJSON(date32(), "[1234, null, 9012]"));
        FillPrimitive(3, -1, 0, date64_buffers_nulls8);
        CheckImport(ArrayFromJSON(date64(), "[86400000, null, -86400000]"));
        FillPrimitive(2, -1, 0, primitive_buffers_nulls8);
        CheckImport(ArrayFromJSON(time64(TimeUnit::NANO), "[123456789, null]"));
        FillPrimitive(2, -1, 0, primitive_buffers_nulls8);
        CheckImport(ArrayFromJSON(duration(TimeUnit::NANO), "[123456789, null]"));
        FillPrimitive(3, -1, 0, primitive_buffers_nulls7);
        CheckImport(ArrayFromJSON(month_interval(), "[1234, null, 9012]"));
        FillPrimitive(2, -1, 0, primitive_buffers_nulls7);
        CheckImport(ArrayFromJSON(day_time_interval(), "[[1234, 5678], null]"));
        FillPrimitive(3, -1, 0, timestamp_buffers_nulls1);
        CheckImport(ArrayFromJSON(timestamp(TimeUnit::SECOND, "UTC+2"),
                                  R"(["1970-01-01",null,"1900-02-28"])"));
    }

    TEST_F(TestArrayImport, Null) {
        // Arrow C++ used to export null arrays with a null bitmap buffer
        for (const int64_t n_buffers: {0, 1}) {
            const void *buffers[] = {nullptr};
            c_struct_.length = 3;
            c_struct_.null_count = 3;
            c_struct_.offset = 0;
            c_struct_.buffers = buffers;
            c_struct_.n_buffers = n_buffers;
            CheckImport(ArrayFromJSON(null(), "[null, null, null]"));
        }
    }
} // namespace arrow
