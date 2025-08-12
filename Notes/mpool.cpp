//在这个文件中，我想探讨内存池的实现,

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <thread>
#include <cassert>
#include <sstream>
#include <shared_mutex>

// 常量定义
const size_t DEFAULT_ALIGNMENT = 8;  // 默认对齐大小
const size_t MIN_BLOCK_SIZE = 16;    // 最小块大小
const size_t MAX_BLOCK_SIZE = 1024 * 1024;  // 最大块大小
const double DEFAULT_GROWTH_FACTOR = 2.0;    // 默认增长因子

// 错误类型枚举
enum class ErrorType {
    OUT_OF_MEMORY,
    INVALID_POINTER,
    POOL_FULL,
    INVALID_ALIGNMENT,
    UNKNOWN_ERROR
};

// 错误处理策略枚举
enum class ErrorHandlingStrategy {
    THROW_EXCEPTION,  // 抛出异常
    RETURN_NULLPTR,    // 返回空指针
    LOG_ERROR,         // 记录错误日志
    TERMINATE          // 终止程序
};

// 健康状态枚举
enum class HealthStatus {
    HEALTHY,   // 健康
    WARNING,   // 警告
    CRITICAL   // 严重
};

// 内存使用情况结构体
struct MemoryUsage {
    size_t total = 0;
    size_t used = 0;
    size_t free = 0;
    double usage_percent = 0.0;
};

// 性能指标结构体
struct PerformanceMetrics {
    double avg_alloc_time_ns = 0.0;
    double avg_dealloc_time_ns = 0.0;
    size_t max_alloc_time_ns = 0;
    size_t max_dealloc_time_ns = 0;
    size_t allocation_count = 0;
    size_t deallocation_count = 0;
};

// 错误统计结构体
struct ErrorStats {
    size_t allocation_failures = 0;
    size_t deallocation_failures = 0;
    size_t invalid_pointer_errors = 0;
    double alloc_failure_rate = 0.0;
    double dealloc_failure_rate = 0.0;
};

// 健康报告结构体
struct HealthReport {
    HealthStatus status = HealthStatus::HEALTHY;
    std::string message;
    double fragmentation_rate = 0.0;
    double memory_usage = 0.0;
    double error_rate = 0.0;
};

// 内存段结构体
struct MemorySegment {
    void* base = nullptr;
    size_t size = 0;
    bool owned = false;  // 是否由内存池管理
    
    MemorySegment(void* base_ptr = nullptr, size_t seg_size = 0, bool is_owned = false)
        : base(base_ptr), size(seg_size), owned(is_owned) {}
};

// 内存池异常类
class MemoryPoolException : public std::runtime_error {
private:
    ErrorType error_type;
    
public:
    MemoryPoolException(const std::string& message, ErrorType type = ErrorType::UNKNOWN_ERROR)
        : std::runtime_error(message), error_type(type) {}
    
    ErrorType get_error_type() const {
        return error_type;
    }
};

// 内存块描述符类
class MemoryBlockDescriptor {
private:
    void* address;                     // 内存块地址
    size_t size;                       // 内存块大小
    bool allocated;                    // 是否已分配
    MemoryBlockDescriptor* next;       // 下一个块（用于链表）
    
public:
    MemoryBlockDescriptor(void* addr = nullptr, size_t sz = 0, bool alloc = false)
        : address(addr), size(sz), allocated(alloc), next(nullptr) {}
    
    // 基本属性
    void* get_address() const {
        return address;
    }
    
    size_t get_size() const {
        return size;
    }
    
    bool is_allocated() const {
        return allocated;
    }
    
    void set_allocated(bool alloc) {
        allocated = alloc;
    }
    
    // 链表操作
    MemoryBlockDescriptor* get_next() const {
        return next;
    }
    
    void set_next(MemoryBlockDescriptor* next_block) {
        next = next_block;
    }
    
    // 伙伴系统
    void* calculate_buddy_address() const {
        if (!address) {
            return nullptr;
        }
        
        uintptr_t addr = reinterpret_cast<uintptr_t>(address);
        uintptr_t buddy_addr = addr ^ size;
        
        return reinterpret_cast<void*>(buddy_addr);
    }
};

// 自由链表类
class FreeList {
private:
    size_t block_size;                 // 内存块大小
    MemoryBlockDescriptor* head;      // 链表头
    size_t block_count;                // 内存块数量
    mutable std::mutex list_mutex;     // 链表锁
    
public:
    FreeList(size_t size = 0) : block_size(size), head(nullptr), block_count(0) {}
    
    ~FreeList() {
        clear();
    }
    
    // 禁用拷贝构造和赋值操作
    FreeList(const FreeList&) = delete;
    FreeList& operator=(const FreeList&) = delete;
    
    // 链表操作
    void push(MemoryBlockDescriptor* block) {
        if (!block) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(list_mutex);
        
        block->set_next(head);
        head = block;
        block_count++;
    }
    
    MemoryBlockDescriptor* pop() {
        std::lock_guard<std::mutex> lock(list_mutex);
        
        if (!head) {
            return nullptr;
        }
        
        MemoryBlockDescriptor* block = head;
        head = head->get_next();
        block_count--;
        
        return block;
    }
    
    bool is_empty() const {
        std::lock_guard<std::mutex> lock(list_mutex);
        return head == nullptr;
    }
    
    bool remove(MemoryBlockDescriptor* block) {
        if (!block || !head) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(list_mutex);
        
        // 如果要移除的是头节点
        if (head == block) {
            head = head->get_next();
            block_count--;
            return true;
        }
        
        // 查找要移除的节点
        MemoryBlockDescriptor* current = head;
        while (current->get_next() && current->get_next() != block) {
            current = current->get_next();
        }
        
        if (current->get_next() == block) {
            current->set_next(block->get_next());
            block_count--;
            return true;
        }
        
        return false;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(list_mutex);
        
        MemoryBlockDescriptor* current = head;
        while (current) {
            MemoryBlockDescriptor* next = current->get_next();
            delete current;
            current = next;
        }
        
        head = nullptr;
        block_count = 0;
    }
    
    // 查询方法
    size_t get_block_size() const {
        return block_size;
    }
    
    size_t get_block_count() const {
        std::lock_guard<std::mutex> lock(list_mutex);
        return block_count;
    }
    
    MemoryBlockDescriptor* get_head() const {
        return head;
    }
    
    void set_head(MemoryBlockDescriptor* new_head) {
        std::lock_guard<std::mutex> lock(list_mutex);
        head = new_head;
    }
};

// 内存对齐工具类
class MemoryAlignment {
public:
    // 获取系统的默认对齐大小
    static size_t get_system_alignment() {
        // 使用常见的对齐大小作为替代
        return sizeof(void*);
    }
    
    // 获取指定类型的对齐要求
    template<typename T>
    static size_t get_type_alignment() {
        return alignof(T);
    }
    
    // 计算向上对齐的大小
    static size_t align_up(size_t size, size_t alignment) {
        if (alignment == 0) {
            return size;
        }
        return (size + alignment - 1) & ~(alignment - 1);
    }
    
    // 计算向下对齐的大小
    static size_t align_down(size_t size, size_t alignment) {
        if (alignment == 0) {
            return size;
        }
        return size & ~(alignment - 1);
    }
    
    // 检查地址是否对齐
    static bool is_aligned(void* ptr, size_t alignment) {
        return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
    }
    
    // 获取下一个对齐地址
    static void* get_aligned_address(void* ptr, size_t alignment) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t aligned_addr = align_up(addr, alignment);
        return reinterpret_cast<void*>(aligned_addr);
    }
    
    // 获取对齐所需的偏移量
    static size_t get_alignment_offset(void* ptr, size_t alignment) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        return (alignment - (addr % alignment)) % alignment;
    }
};

// 统计信息类
class PoolStats {
private:
    // 基本统计
    size_t total_memory;          // 总内存大小
    size_t used_memory;           // 已使用内存大小
    size_t free_memory;           // 空闲内存大小
    size_t allocation_count;      // 分配次数
    size_t deallocation_count;    // 释放次数
    size_t fragment_count;        // 碎片数量
    
    // 性能统计
    std::chrono::nanoseconds total_alloc_time;  // 总分配时间
    std::chrono::nanoseconds total_dealloc_time; // 总释放时间
    size_t max_alloc_time;        // 最大单次分配时间
    size_t max_dealloc_time;      // 最大单次释放时间
    
    // 历史统计
    size_t peak_memory_usage;     // 峰值内存使用量
    size_t peak_allocation_count; // 峰值分配次数
    std::chrono::system_clock::time_point creation_time; // 创建时间
    std::chrono::system_clock::time_point last_access_time; // 最后访问时间
    
    // 错误统计
    size_t allocation_failures;   // 分配失败次数
    size_t deallocation_failures; // 释放失败次数
    size_t invalid_pointer_errors; // 无效指针错误次数
    
    // 块大小分布
    std::map<size_t, size_t> block_size_distribution; // 块大小分布
    
    mutable std::shared_mutex stats_mutex; // 读写锁
    
public:
    PoolStats() 
        : total_memory(0), used_memory(0), free_memory(0), 
          allocation_count(0), deallocation_count(0), fragment_count(0),
          total_alloc_time(0), total_dealloc_time(0),
          max_alloc_time(0), max_dealloc_time(0),
          peak_memory_usage(0), peak_allocation_count(0),
          allocation_failures(0), deallocation_failures(0),
          invalid_pointer_errors(0) {
        creation_time = std::chrono::system_clock::now();
        last_access_time = creation_time;
    }
    
    // 禁用拷贝构造和赋值操作
    PoolStats(const PoolStats&) = delete;
    PoolStats& operator=(const PoolStats&) = delete;
    
    // 基本统计方法
    size_t get_total_memory() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return total_memory;
    }
    
    size_t get_used_memory() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return used_memory;
    }
    
    size_t get_free_memory() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return free_memory;
    }
    
    size_t get_allocation_count() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return allocation_count;
    }
    
    size_t get_deallocation_count() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return deallocation_count;
    }
    
    size_t get_fragment_count() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return fragment_count;
    }
    
    // 性能统计方法
    std::chrono::nanoseconds get_total_alloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return total_alloc_time;
    }
    
    std::chrono::nanoseconds get_total_dealloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return total_dealloc_time;
    }
    
    size_t get_max_alloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return max_alloc_time;
    }
    
    size_t get_max_dealloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return max_dealloc_time;
    }
    
    double get_average_alloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        if (allocation_count == 0) {
            return 0.0;
        }
        return static_cast<double>(total_alloc_time.count()) / allocation_count;
    }
    
    double get_average_dealloc_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        if (deallocation_count == 0) {
            return 0.0;
        }
        return static_cast<double>(total_dealloc_time.count()) / deallocation_count;
    }
    
    // 历史统计方法
    size_t get_peak_memory_usage() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return peak_memory_usage;
    }
    
    size_t get_peak_allocation_count() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return peak_allocation_count;
    }
    
    std::chrono::system_clock::time_point get_creation_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return creation_time;
    }
    
    std::chrono::system_clock::time_point get_last_access_time() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return last_access_time;
    }
    
    std::chrono::duration<double> get_uptime() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return std::chrono::system_clock::now() - creation_time;
    }
    
    // 错误统计方法
    size_t get_allocation_failures() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return allocation_failures;
    }
    
    size_t get_deallocation_failures() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return deallocation_failures;
    }
    
    size_t get_invalid_pointer_errors() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        return invalid_pointer_errors;
    }
    
    double get_allocation_failure_rate() const {
        std::shared_lock<std::shared_mutex> lock(stats_mutex);
        if (allocation_count == 0) {
            return 0.0;
        }
    }
    
    double get_deallocation_failure_rate() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        if (deallocation_count == 0) {
            return 0.0;
        }
        return static_cast<double>(deallocation_failures) / deallocation_count;
    }
    
    // 块大小分布
    const std::map<size_t, size_t> get_block_size_distribution() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        return block_size_distribution;
    }
    
    // 使用率和碎片率
    double get_memory_usage() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        if (total_memory == 0) {
            return 0.0;
        }
        return static_cast<double>(used_memory) / total_memory * 100.0;
    }
    
    double get_fragmentation_rate() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        if (free_memory == 0) {
            return 0.0;
        }
        return static_cast<double>(fragment_count) / (free_memory / MIN_BLOCK_SIZE) * 100.0;
    }
    
    // 更新方法
    void update_allocation(size_t size, std::chrono::nanoseconds duration) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        allocation_count++;
        used_memory += size;
        free_memory -= size;
        total_alloc_time += duration;
        
        if (duration.count() > max_alloc_time) {
            max_alloc_time = duration.count();
        }
        
        if (used_memory > peak_memory_usage) {
            peak_memory_usage = used_memory;
        }
        
        if (allocation_count > peak_allocation_count) {
            peak_allocation_count = allocation_count;
        }
        
        last_access_time = std::chrono::system_clock::now();
        
        // 更新块大小分布
        block_size_distribution[size]++;
    }
    
    void update_deallocation(size_t size, std::chrono::nanoseconds duration) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        deallocation_count++;
        used_memory -= size;
        free_memory += size;
        total_dealloc_time += duration;
        
        if (duration.count() > max_dealloc_time) {
            max_dealloc_time = duration.count();
        }
        
        last_access_time = std::chrono::system_clock::now();
    }
    
    void update_allocation_failure() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        allocation_failures++;
        last_access_time = std::chrono::system_clock::now();
    }
    
    void update_deallocation_failure() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        deallocation_failures++;
        last_access_time = std::chrono::system_clock::now();
    }
    
    void update_invalid_pointer_error() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        invalid_pointer_errors++;
        last_access_time = std::chrono::system_clock::now();
    }
    
    void update_fragmentation(int delta) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        fragment_count += delta;
        last_access_time = std::chrono::system_clock::now();
    }
    
    void set_total_memory(size_t size) {
        std::lock_guard<std::mutex> lock(stats_mutex);
        total_memory = size;
        free_memory = size;
        last_access_time = std::chrono::system_clock::now();
    }
    
    // 重置和摘要
    void reset() {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        total_memory = 0;
        used_memory = 0;
        free_memory = 0;
        allocation_count = 0;
        deallocation_count = 0;
        fragment_count = 0;
        total_alloc_time = std::chrono::nanoseconds(0);
        total_dealloc_time = std::chrono::nanoseconds(0);
        max_alloc_time = 0;
        max_dealloc_time = 0;
        peak_memory_usage = 0;
        peak_allocation_count = 0;
        allocation_failures = 0;
        deallocation_failures = 0;
        invalid_pointer_errors = 0;
        block_size_distribution.clear();
        
        creation_time = std::chrono::system_clock::now();
        last_access_time = creation_time;
    }
    
    std::string get_summary() const {
        std::lock_guard<std::mutex> lock(stats_mutex);
        
        std::ostringstream oss;
        oss << "Memory Pool Statistics:\n";
        oss << "  Total Memory: " << total_memory << " bytes\n";
        oss << "  Used Memory: " << used_memory << " bytes (" << get_memory_usage() << "%)\n";
        oss << "  Free Memory: " << free_memory << " bytes\n";
        oss << "  Allocations: " << allocation_count << "\n";
        oss << "  Deallocations: " << deallocation_count << "\n";
        oss << "  Fragments: " << fragment_count << " (" << get_fragmentation_rate() << "%)\n";
        oss << "  Allocation Failures: " << allocation_failures << " (" << get_allocation_failure_rate() * 100 << "%)\n";
        oss << "  Average Alloc Time: " << get_average_alloc_time() << " ns\n";
        oss << "  Average Dealloc Time: " << get_average_dealloc_time() << " ns\n";
        oss << "  Uptime: " << get_uptime().count() << " seconds\n";
        
        return oss.str();
    }
};

// 内存池类
class MemoryPool {
private:
    // 基本配置
    void* pool_base;                    // 内存池基地址
    size_t pool_size;                   // 内存池大小
    size_t min_block_size;              // 最小块大小
    size_t max_block_size;              // 最大块大小
    bool thread_safe;                   // 是否启用线程安全
    double growth_factor;               // 增长因子
    size_t max_memory_limit;            // 最大内存限制
    
    // 线程安全机制
    mutable std::mutex pool_mutex;      // 互斥锁
    mutable std::mutex stats_mutex;     // 统计信息锁
    std::vector<std::mutex> free_list_mutexes; // 自由链表锁
    std::atomic<size_t> atomic_allocation_count{0}; // 原子分配计数器
    std::atomic<size_t> atomic_deallocation_count{0}; // 原子释放计数器
    
    // 内存管理
    FreeList* free_lists;               // 自由链表数组
    size_t free_list_count;             // 自由链表数量
    std::vector<MemorySegment> memory_segments; // 内存段列表
    
    // 统计信息
    mutable PoolStats stats;            // 统计信息
    
    // 错误处理
    ErrorHandlingStrategy error_strategy; // 错误处理策略
    std::function<void(const std::string&)> error_logger; // 错误日志回调
    
    // 锁管理辅助类，确保异常安全
    class ScopedLock {
    private:
        std::mutex& mutex;
        bool locked;
        
    public:
        explicit ScopedLock(std::mutex& m) : mutex(m), locked(true) {
            mutex.lock();
        }
        
        ~ScopedLock() {
            if (locked) {
                mutex.unlock();
            }
        }
        
        void unlock() {
            if (locked) {
                mutex.unlock();
                locked = false;
            }
        }
    };
    
public:
    // 构造函数和析构函数
    MemoryPool(size_t initial_size = 1024 * 1024, 
               size_t min_blk_size = MIN_BLOCK_SIZE, 
               size_t max_blk_size = MAX_BLOCK_SIZE, 
               bool safe = true, 
               double factor = DEFAULT_GROWTH_FACTOR)
        : pool_base(nullptr), pool_size(initial_size), 
          min_block_size(min_blk_size), max_block_size(max_blk_size),
          thread_safe(safe), growth_factor(factor), max_memory_limit(0),
          free_lists(nullptr), free_list_count(0),
          error_strategy(ErrorHandlingStrategy::THROW_EXCEPTION) {
        
        // 验证参数
        if (min_block_size == 0 || (min_block_size & (min_block_size - 1)) != 0) {
            throw MemoryPoolException("Minimum block size must be a power of 2", ErrorType::INVALID_ALIGNMENT);
        }
        
        if (max_block_size < min_block_size || (max_block_size & (max_block_size - 1)) != 0) {
            throw MemoryPoolException("Maximum block size must be a power of 2 and greater than or equal to minimum block size", ErrorType::INVALID_ALIGNMENT);
        }
        
        // 计算自由链表数量
        free_list_count = static_cast<size_t>(log2(max_block_size) - log2(min_block_size)) + 1;
        
        // 创建自由链表数组
        free_lists = new FreeList[free_list_count];
        for (size_t i = 0; i < free_list_count; ++i) {
            size_t block_size = min_block_size << i;
            free_lists[i] = FreeList(block_size);
        }
        
        // 初始化自由链表锁
        if (thread_safe) {
            free_list_mutexes.resize(free_list_count);
        }
        
        // 初始化内存池
        initialize_pool(initial_size);
    }
    
    ~MemoryPool() {
        // 释放所有内存段
        release_all_segments();
        
        // 释放自由链表数组
        delete[] free_lists;
    }
    
    // 禁用拷贝构造和赋值操作
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // 内存分配和释放
    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) {
        if (size == 0) {
            return nullptr;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            
            try {
                void* result = allocate_from_pool(size, alignment);
                
                // 使用原子操作更新计数器
                atomic_allocation_count.fetch_add(1, std::memory_order_relaxed);
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
                
                // 更新统计信息
                size_t actual_size = calculate_block_size(size);
                stats.update_allocation(actual_size, duration);
                
                return result;
            } catch (...) {
                // 确保异常情况下锁能正确释放
                stats.update_allocation_failure();
                handle_error("Memory allocation failed", ErrorType::OUT_OF_MEMORY);
                throw;
            }
        } else {
            // 单线程模式，无需加锁
            try {
                void* result = allocate_from_pool(size, alignment);
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
                
                // 更新统计信息
                size_t actual_size = calculate_block_size(size);
                stats.update_allocation(actual_size, duration);
                
                return result;
            } catch (...) {
                stats.update_allocation_failure();
                handle_error("Memory allocation failed", ErrorType::OUT_OF_MEMORY);
                throw;
            }
        }
    }
    
    template<typename T>
    T* allocate_type(size_t count = 1) {
        size_t size = sizeof(T) * count;
        size_t alignment = alignof(T);
        return static_cast<T*>(allocate(size, alignment));
    }
    
    void deallocate(void* ptr) {
        if (!ptr) {
            return;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            
            try {
                deallocate_from_pool(ptr);
                
                // 使用原子操作更新计数器
                atomic_deallocation_count.fetch_add(1, std::memory_order_relaxed);
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
                
                // 更新统计信息
                size_t size = get_block_size(ptr);
                stats.update_deallocation(size, duration);
                
            } catch (...) {
                // 确保异常情况下锁能正确释放
                stats.update_deallocation_failure();
                handle_error("Memory deallocation failed", ErrorType::INVALID_POINTER);
                throw;
            }
        } else {
            // 单线程模式，无需加锁
            try {
                deallocate_from_pool(ptr);
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
                
                // 更新统计信息
                size_t size = get_block_size(ptr);
                stats.update_deallocation(size, duration);
                
            } catch (...) {
                stats.update_deallocation_failure();
                handle_error("Memory deallocation failed", ErrorType::INVALID_POINTER);
                throw;
            }
        }
    }
    
    void* safe_allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) {
        try {
            return allocate(size, alignment);
        } catch (const MemoryPoolException&) {
            return nullptr;
        }
    }
    
    bool safe_deallocate(void* ptr) {
        try {
            deallocate(ptr);
            return true;
        } catch (const MemoryPoolException&) {
            return false;
        }
    }
    
    // 内存池管理
    void reset() {
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            reset_pool();
        } else {
            reset_pool();
        }
    }
    
    bool is_valid_pointer(void* ptr) const {
        if (!ptr) {
            return false;
        }
        
        if (thread_safe) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            return is_valid_pointer_internal(ptr);
        } else {
            return is_valid_pointer_internal(ptr);
        }
    }
    
    size_t get_block_size(void* ptr) const {
        if (!ptr) {
            return 0;
        }
        
        if (thread_safe) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            return get_block_size_internal(ptr);
        } else {
            return get_block_size_internal(ptr);
        }
    }
    
    // 线程安全控制
    void set_thread_safe(bool enabled) {
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            thread_safe = enabled;
        } else {
            thread_safe = enabled;
        }
    }
    
    bool is_thread_safe() const {
        return thread_safe;
    }
    
    // 统计和监控
    PoolStats get_stats() const {
        if (thread_safe) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            return stats;
        } else {
            return stats;
        }
    }
    
    MemoryUsage get_memory_usage() const {
        PoolStats current_stats = get_stats();
        
        MemoryUsage usage;
        usage.total = current_stats.get_total_memory();
        usage.used = current_stats.get_used_memory();
        usage.free = current_stats.get_free_memory();
        usage.usage_percent = current_stats.get_memory_usage();
        
        return usage;
    }
    
    PerformanceMetrics get_performance_metrics() const {
        PoolStats current_stats = get_stats();
        
        PerformanceMetrics metrics;
        metrics.avg_alloc_time_ns = current_stats.get_average_alloc_time();
        metrics.avg_dealloc_time_ns = current_stats.get_average_dealloc_time();
        metrics.max_alloc_time_ns = current_stats.get_max_alloc_time();
        metrics.max_dealloc_time_ns = current_stats.get_max_dealloc_time();
        metrics.allocation_count = current_stats.get_allocation_count();
        metrics.deallocation_count = current_stats.get_deallocation_count();
        
        return metrics;
    }
    
    ErrorStats get_error_stats() const {
        PoolStats current_stats = get_stats();
        
        ErrorStats error_stats;
        error_stats.allocation_failures = current_stats.get_allocation_failures();
        error_stats.deallocation_failures = current_stats.get_deallocation_failures();
        error_stats.invalid_pointer_errors = current_stats.get_invalid_pointer_errors();
        error_stats.alloc_failure_rate = current_stats.get_allocation_failure_rate();
        error_stats.dealloc_failure_rate = current_stats.get_deallocation_failure_rate();
        
        return error_stats;
    }
    
    HealthReport get_health_report() const {
        PoolStats current_stats = get_stats();
        
        HealthReport report;
        report.fragmentation_rate = current_stats.get_fragmentation_rate();
        report.memory_usage = current_stats.get_memory_usage();
        report.error_rate = (current_stats.get_allocation_failure_rate() + 
                            current_stats.get_deallocation_failure_rate()) * 100.0;
        
        // 根据各种指标确定健康状态
        if (report.error_rate > 10.0 || report.fragmentation_rate > 50.0) {
            report.status = HealthStatus::CRITICAL;
            report.message = "Memory pool is in critical condition";
        } else if (report.error_rate > 5.0 || report.fragmentation_rate > 30.0) {
            report.status = HealthStatus::WARNING;
            report.message = "Memory pool has warnings";
        } else {
            report.status = HealthStatus::HEALTHY;
            report.message = "Memory pool is healthy";
        }
        
        return report;
    }
    
    std::string get_detailed_report() const {
        PoolStats current_stats = get_stats();
        return current_stats.get_summary();
    }
    
    void reset_stats() {
        if (thread_safe) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            stats.reset();
        } else {
            stats.reset();
        }
    }
    
    // 错误处理
    void set_error_handling_strategy(ErrorHandlingStrategy strategy) {
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            error_strategy = strategy;
        } else {
            error_strategy = strategy;
        }
    }
    
    void set_error_logger(std::function<void(const std::string&)> logger) {
        if (thread_safe) {
            ScopedLock pool_lock(pool_mutex);
            error_logger = logger;
        } else {
            error_logger = logger;
        }
    }
    
private:
    // 内部实现方法
    void* allocate_from_pool(size_t size, size_t alignment = DEFAULT_ALIGNMENT) {
        // 计算需要的块大小
        size_t block_size = calculate_block_size(size);
        
        // 检查是否超过最大块大小
        if (block_size > max_block_size) {
            handle_error("Requested size exceeds maximum block size", ErrorType::OUT_OF_MEMORY);
            throw MemoryPoolException("Requested size exceeds maximum block size", ErrorType::OUT_OF_MEMORY);
        }
        
        // 计算对应的自由链表索引
        size_t list_index = static_cast<size_t>(log2(block_size) - log2(min_block_size));
        
        // 尝试从对应的自由链表获取块
        void* result = allocate_from_free_list(list_index, block_size);
        
        if (!result) {
            // 如果没有可用块，尝试扩展内存池
            expand_pool(block_size);
            
            // 扩展后再次尝试分配
            result = allocate_from_free_list(list_index, block_size);
            
            if (!result) {
                handle_error("Failed to allocate memory after pool expansion", ErrorType::OUT_OF_MEMORY);
                throw MemoryPoolException("Failed to allocate memory after pool expansion", ErrorType::OUT_OF_MEMORY);
            }
        }
        
        return result;
    }
    
    void* allocate_from_free_list(size_t list_index, size_t block_size) {
        // 检查索引是否有效
        if (list_index >= free_list_count) {
            return nullptr;
        }
        
        // 尝试从当前链表获取块
        MemoryBlockDescriptor* block = free_lists[list_index].pop();
        
        if (block) {
            // 标记为已分配
            block->set_allocated(true);
            return block->get_address();
        }
        
        // 如果当前链表为空，尝试从更大的链表分割块
        for (size_t i = list_index + 1; i < free_list_count; ++i) {
            MemoryBlockDescriptor* larger_block = free_lists[i].pop();
            
            if (larger_block) {
                // 分割大块
                split_block(larger_block, list_index);
                
                // 再次尝试从当前链表获取块
                block = free_lists[list_index].pop();
                
                if (block) {
                    // 标记为已分配
                    block->set_allocated(true);
                    return block->get_address();
                }
            }
        }
        
        return nullptr;
    }
    
    void deallocate_from_pool(void* ptr) {
        // 检查指针是否有效
        if (!is_valid_pointer_internal(ptr)) {
            stats.update_invalid_pointer_error();
            handle_error("Invalid pointer passed to deallocate", ErrorType::INVALID_POINTER);
            throw MemoryPoolException("Invalid pointer passed to deallocate", ErrorType::INVALID_POINTER);
        }
        
        // 获取块大小
        size_t size = get_block_size_internal(ptr);
        
        // 计算对应的自由链表索引
        size_t list_index = static_cast<size_t>(log2(size) - log2(min_block_size));
        
        // 创建新的块描述符
        MemoryBlockDescriptor* block = new MemoryBlockDescriptor(ptr, size, false);
        
        // 将块添加到自由链表
        free_lists[list_index].push(block);
        
        // 尝试合并伙伴块
        merge_blocks(block);
    }
    
    void split_block(MemoryBlockDescriptor* block, size_t target_list_index) {
        // 获取当前块的大小
        size_t current_size = block->get_size();
        size_t current_list_index = static_cast<size_t>(log2(current_size) - log2(min_block_size));
        
        // 如果当前链表已经是目标链表，无需分割
        if (current_list_index <= target_list_index) {
            free_lists[current_list_index].push(block);
            return;
        }
        
        // 分割块
        size_t new_size = current_size / 2;
        void* addr = block->get_address();
        
        // 创建第一个子块
        MemoryBlockDescriptor* first_block = new MemoryBlockDescriptor(addr, new_size, false);
        
        // 创建第二个子块（伙伴块）
        void* second_addr = static_cast<char*>(addr) + new_size;
        MemoryBlockDescriptor* second_block = new MemoryBlockDescriptor(second_addr, new_size, false);
        
        // 将子块添加到对应的自由链表
        size_t new_list_index = static_cast<size_t>(log2(new_size) - log2(min_block_size));
        free_lists[new_list_index].push(first_block);
        free_lists[new_list_index].push(second_block);
        
        // 更新碎片统计
        stats.update_fragmentation(1);
        
        // 释放原始块
        delete block;
        
        // 递归分割，直到达到目标大小
        if (new_list_index > target_list_index) {
            split_block(first_block, target_list_index);
        }
    }
    
    void merge_blocks(MemoryBlockDescriptor* block) {
        if (block->is_allocated()) {
            return; // 已分配的块不能合并
        }
        
        // 查找伙伴块
        MemoryBlockDescriptor* buddy = find_buddy(block);
        
        // 检查伙伴块是否存在且空闲
        if (buddy && !buddy->is_allocated() && buddy->get_size() == block->get_size()) {
            // 从自由链表中移除伙伴块
            size_t list_index = static_cast<size_t>(log2(block->get_size()) - log2(min_block_size));
            free_lists[list_index].remove(buddy);
            
            // 创建新的合并块
            void* new_addr = std::min(block->get_address(), buddy->get_address());
            size_t new_size = block->get_size() * 2;
            MemoryBlockDescriptor* merged_block = new MemoryBlockDescriptor(new_addr, new_size, false);
            
            // 将合并块添加到对应的自由链表
            size_t new_list_index = static_cast<size_t>(log2(new_size) - log2(min_block_size));
            if (new_list_index < free_list_count) {
                free_lists[new_list_index].push(merged_block);
            }
            
            // 释放原来的块描述符
            delete block;
            delete buddy;
            
            // 更新碎片统计
            stats.update_fragmentation(-1);
            
            // 递归尝试继续合并
            merge_blocks(merged_block);
        }
    }
    
    MemoryBlockDescriptor* find_buddy(MemoryBlockDescriptor* block) {
        if (!block) {
            return nullptr;
        }
        
        // 计算伙伴地址
        void* buddy_addr = block->calculate_buddy_address();
        
        // 在对应的自由链表中查找伙伴块
        size_t list_index = static_cast<size_t>(log2(block->get_size()) - log2(min_block_size));
        
        MemoryBlockDescriptor* current = free_lists[list_index].get_head();
        while (current) {
            if (current->get_address() == buddy_addr) {
                return current;
            }
            current = current->get_next();
        }
        
        return nullptr;
    }
    
    size_t calculate_block_size(size_t requested_size) {
        // 确保块大小至少为最小块大小
        size_t size = std::max(requested_size, min_block_size);
        
        // 计算向上对齐的2的幂次方
        size_t power = 1;
        while (power < size) {
            power <<= 1;
        }
        
        return power;
    }
    
    void initialize_pool(size_t initial_size) {
        // 分配初始内存
        void* memory = allocate_system_memory(initial_size);
        
        if (!memory) {
            handle_error("Failed to allocate initial memory pool", ErrorType::OUT_OF_MEMORY);
            throw MemoryPoolException("Failed to allocate initial memory pool", ErrorType::OUT_OF_MEMORY);
        }
        
        // 添加到内存段列表
        add_memory_segment(memory, initial_size);
        
        // 初始化自由链表
        initialize_free_lists();
    }
    
    void initialize_free_lists() {
        // 清空所有自由链表
        for (size_t i = 0; i < free_list_count; ++i) {
            free_lists[i].clear();
        }
        
        // 遍历所有内存段，初始化自由链表
        for (const auto& segment : memory_segments) {
            initialize_segment(segment.base, segment.size);
        }
    }
    
    void initialize_segment(void* base, size_t size) {
        // 将内存段分割为最大块
        size_t block_size = max_block_size;
        size_t remaining_size = size;
        void* current_addr = base;
        
        while (remaining_size >= block_size) {
            // 创建块描述符
            MemoryBlockDescriptor* block = new MemoryBlockDescriptor(current_addr, block_size, false);
            
            // 计算对应的自由链表索引
            size_t list_index = static_cast<size_t>(log2(block_size) - log2(min_block_size));
            
            // 将块添加到自由链表
            free_lists[list_index].push(block);
            
            // 移动到下一个块
            current_addr = static_cast<char*>(current_addr) + block_size;
            remaining_size -= block_size;
        }
        
        // 处理剩余的小块
        if (remaining_size >= min_block_size) {
            // 为剩余部分找到合适的块大小
            size_t remaining_block_size = min_block_size;
            while (remaining_block_size * 2 <= remaining_size) {
                remaining_block_size *= 2;
            }
            
            // 创建块描述符
            MemoryBlockDescriptor* block = new MemoryBlockDescriptor(current_addr, remaining_block_size, false);
            
            // 计算对应的自由链表索引
            size_t list_index = static_cast<size_t>(log2(remaining_block_size) - log2(min_block_size));
            
            // 将块添加到自由链表
            free_lists[list_index].push(block);
        }
    }
    
    void add_memory_segment(void* base, size_t size) {
        memory_segments.emplace_back(base, size, true);
        stats.set_total_memory(stats.get_total_memory() + size);
    }
    
    void release_all_segments() {
        for (auto& segment : memory_segments) {
            if (segment.owned && segment.base) {
                deallocate_system_memory(segment.base, segment.size);
            }
        }
        memory_segments.clear();
    }
    
    void* allocate_system_memory(size_t size) {
        // 使用系统分配函数
        void* memory = std::malloc(size);
        
        if (memory) {
            // 清零内存
            std::memset(memory, 0, size);
        }
        
        return memory;
    }
    
    void deallocate_system_memory(void* ptr, size_t size) {
        // 使用系统释放函数
        std::free(ptr);
    }
    
    void expand_pool(size_t required_size) {
        // 计算需要扩展的大小
        size_t current_total = 0;
        for (const auto& segment : memory_segments) {
            current_total += segment.size;
        }
        
        size_t expand_size = static_cast<size_t>(current_total * (growth_factor - 1.0));
        expand_size = std::max(expand_size, required_size);
        
        // 确保扩展大小是最大块大小的整数倍
        expand_size = MemoryAlignment::align_up(expand_size, max_block_size);
        
        // 检查是否超过最大内存限制
        if (max_memory_limit > 0 && current_total + expand_size > max_memory_limit) {
            handle_error("Memory pool has reached maximum size limit", ErrorType::POOL_FULL);
            throw MemoryPoolException("Memory pool has reached maximum size limit", ErrorType::POOL_FULL);
        }
        
        // 分配新的内存段
        void* new_segment = allocate_system_memory(expand_size);
        
        if (!new_segment) {
            handle_error("Failed to allocate system memory for pool expansion", ErrorType::OUT_OF_MEMORY);
            throw MemoryPoolException("Failed to allocate system memory for pool expansion", ErrorType::OUT_OF_MEMORY);
        }
        
        // 添加到内存段列表
        add_memory_segment(new_segment, expand_size);
        
        // 初始化新段的自由链表
        initialize_segment(new_segment, expand_size);
    }
    
    void reset_pool() {
        // 清空所有自由链表
        for (size_t i = 0; i < free_list_count; ++i) {
            free_lists[i].clear();
        }
        
        // 重新初始化自由链表
        initialize_free_lists();
        
        // 重置统计信息
        stats.reset();
    }
    
    bool is_valid_pointer_internal(void* ptr) const {
        // 检查指针是否在任何内存段范围内
        for (const auto& segment : memory_segments) {
            char* base = static_cast<char*>(segment.base);
            char* target = static_cast<char*>(ptr);
            
            if (target >= base && target < base + segment.size) {
                return true;
            }
        }
        
        return false;
    }
    
    size_t get_block_size_internal(void* ptr) const {
        // 这是一个简化的实现，实际应用中可能需要更复杂的机制来存储和检索块大小
        // 例如，可以在每个块前存储块大小的元数据
        
        // 遍历所有自由链表，查找包含该指针的块
        for (size_t i = 0; i < free_list_count; ++i) {
            MemoryBlockDescriptor* current = free_lists[i].get_head();
            while (current) {
                if (current->get_address() == ptr) {
                    return current->get_size();
                }
                current = current->get_next();
            }
        }
        
        // 如果没有在自由链表中找到，假设这是一个已分配的块
        // 在实际应用中，可能需要更复杂的机制来处理已分配的块
        return min_block_size;
    }
    
    void handle_error(const std::string& error_msg, ErrorType error_type) {
        // 根据错误处理策略处理错误
        switch (error_strategy) {
            case ErrorHandlingStrategy::THROW_EXCEPTION:
                throw MemoryPoolException(error_msg, error_type);
                
            case ErrorHandlingStrategy::RETURN_NULLPTR:
                // 对于分配操作，返回空指针
                // 对于释放操作，直接返回
                break;
                
            case ErrorHandlingStrategy::LOG_ERROR:
                if (error_logger) {
                    error_logger(error_msg);
                }
                break;
                
            case ErrorHandlingStrategy::TERMINATE:
                std::cerr << "Fatal error: " << error_msg << std::endl;
                std::terminate();
                break;
        }
    }
};

int main() {
    std::cout << "内存池测试程序" << std::endl;
    
    try {
        // 创建内存池
        MemoryPool pool(1024 * 1024, 16, 1024 * 1024, true, 2.0);
        
        // 基本内存分配和释放测试
        std::cout << "\n=== 基本内存分配和释放测试 ===" << std::endl;
        
        // 分配100字节内存
        void* ptr1 = pool.allocate(100);
        std::cout << "分配100字节内存成功: " << ptr1 << std::endl;
        
        // 分配1KB内存
        void* ptr2 = pool.allocate(1024);
        std::cout << "分配1KB内存成功: " << ptr2 << std::endl;
        
        // 分配10KB内存
        void* ptr3 = pool.allocate(10 * 1024);
        std::cout << "分配10KB内存成功: " << ptr3 << std::endl;
        
        // 释放内存
        pool.deallocate(ptr1);
        std::cout << "释放ptr1成功" << std::endl;
        
        pool.deallocate(ptr2);
        std::cout << "释放ptr2成功" << std::endl;
        
        pool.deallocate(ptr3);
        std::cout << "释放ptr3成功" << std::endl;
        
        // 类型化分配测试
        std::cout << "\n=== 类型化分配测试 ===" << std::endl;
        
        // 分配int数组
        int* int_array = pool.allocate_type<int>(10);
        std::cout << "分配int数组成功: " << int_array << std::endl;
        
        // 使用数组
        for (int i = 0; i < 10; ++i) {
            int_array[i] = i * i;
        }
        
        // 打印数组内容
        std::cout << "数组内容: ";
        for (int i = 0; i < 10; ++i) {
            std::cout << int_array[i] << " ";
        }
        std::cout << std::endl;
        
        // 释放数组
        pool.deallocate(int_array);
        std::cout << "释放int数组成功" << std::endl;
        
        // 安全分配测试
        std::cout << "\n=== 安全分配测试 ===" << std::endl;
        
        // 尝试分配过大的内存
        void* big_ptr = pool.safe_allocate(100 * 1024 * 1024); // 100MB
        if (!big_ptr) {
            std::cout << "安全分配失败，返回空指针" << std::endl;
        } else {
            std::cout << "安全分配成功: " << big_ptr << std::endl;
            pool.deallocate(big_ptr);
        }
        
        // 内存池状态测试
        std::cout << "\n=== 内存池状态测试 ===" << std::endl;
        
        // 获取统计信息
        auto stats = pool.get_stats();
        std::cout << "内存池统计信息:" << std::endl;
        std::cout << "  总内存: " << stats.get_total_memory() << " 字节" << std::endl;
        std::cout << "  已使用内存: " << stats.get_used_memory() << " 字节" << std::endl;
        std::cout << "  空闲内存: " << stats.get_free_memory() << " 字节" << std::endl;
        std::cout << "  分配次数: " << stats.get_allocation_count() << std::endl;
        std::cout << "  释放次数: " << stats.get_deallocation_count() << std::endl;
        std::cout << "  内存使用率: " << stats.get_memory_usage() << "%" << std::endl;
        std::cout << "  碎片率: " << stats.get_fragmentation_rate() << "%" << std::endl;
        
        // 获取内存使用情况
        auto usage = pool.get_memory_usage();
        std::cout << "\n内存使用情况:" << std::endl;
        std::cout << "  总内存: " << usage.total << " 字节" << std::endl;
        std::cout << "  已使用内存: " << usage.used << " 字节" << std::endl;
        std::cout << "  空闲内存: " << usage.free << " 字节" << std::endl;
        std::cout << "  内存使用率: " << usage.usage_percent << "%" << std::endl;
        
        // 获取性能指标
        auto metrics = pool.get_performance_metrics();
        std::cout << "\n性能指标:" << std::endl;
        std::cout << "  平均分配时间: " << metrics.avg_alloc_time_ns << " 纳秒" << std::endl;
        std::cout << "  平均释放时间: " << metrics.avg_dealloc_time_ns << " 纳秒" << std::endl;
        std::cout << "  最大分配时间: " << metrics.max_alloc_time_ns << " 纳秒" << std::endl;
        std::cout << "  最大释放时间: " << metrics.max_dealloc_time_ns << " 纳秒" << std::endl;
        std::cout << "  分配次数: " << metrics.allocation_count << std::endl;
        std::cout << "  释放次数: " << metrics.deallocation_count << std::endl;
        
        // 获取健康报告
        auto health = pool.get_health_report();
        std::cout << "\n健康报告:" << std::endl;
        std::cout << "  状态: ";
        switch (health.status) {
            case HealthStatus::HEALTHY:
                std::cout << "健康";
                break;
            case HealthStatus::WARNING:
                std::cout << "警告";
                break;
            case HealthStatus::CRITICAL:
                std::cout << "严重";
                break;
        }
        std::cout << std::endl;
        std::cout << "  消息: " << health.message << std::endl;
        std::cout << "  碎片率: " << health.fragmentation_rate << "%" << std::endl;
        std::cout << "  内存使用率: " << health.memory_usage << "%" << std::endl;
        std::cout << "  错误率: " << health.error_rate << "%" << std::endl;
        
        // 获取详细报告
        std::cout << "\n详细报告:" << std::endl;
        std::cout << pool.get_detailed_report() << std::endl;
        
        // 重置测试
        std::cout << "\n=== 重置测试 ===" << std::endl;
        
        // 分配一些内存
        std::vector<void*> pointers;
        for (int i = 0; i < 5; ++i) {
            void* ptr = pool.allocate((i + 1) * 100);
            pointers.push_back(ptr);
            std::cout << "分配 " << (i + 1) * 100 << " 字节内存: " << ptr << std::endl;
        }
        
        // 查看当前状态
        auto before_reset_stats = pool.get_stats();
        std::cout << "重置前 - 已使用内存: " << before_reset_stats.get_used_memory() << " 字节" << std::endl;
        std::cout << "重置前 - 分配次数: " << before_reset_stats.get_allocation_count() << std::endl;
        
        // 重置内存池
        pool.reset();
        std::cout << "内存池已重置" << std::endl;
        
        // 查看重置后状态
        auto after_reset_stats = pool.get_stats();
        std::cout << "重置后 - 已使用内存: " << after_reset_stats.get_used_memory() << " 字节" << std::endl;
        std::cout << "重置后 - 分配次数: " << after_reset_stats.get_allocation_count() << std::endl;
        
        // 错误处理测试
        std::cout << "\n=== 错误处理测试 ===" << std::endl;
        
        // 设置错误处理策略为返回空指针
        pool.set_error_handling_strategy(ErrorHandlingStrategy::RETURN_NULLPTR);
        
        // 尝试分配过大的内存
        void* error_ptr = pool.safe_allocate(100 * 1024 * 1024); // 100MB
        if (!error_ptr) {
            std::cout << "错误处理测试：分配失败，返回空指针" << std::endl;
        }
        
        // 尝试释放无效指针
        bool result = pool.safe_deallocate(reinterpret_cast<void*>(0x12345678));
        if (!result) {
            std::cout << "错误处理测试：释放失败，返回false" << std::endl;
        }
        
        // 恢复错误处理策略
        pool.set_error_handling_strategy(ErrorHandlingStrategy::THROW_EXCEPTION);
        
        std::cout << "\n内存池测试完成" << std::endl;
        
    } catch (const MemoryPoolException& e) {
        std::cerr << "内存池错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "标准异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "未知异常" << std::endl;
        return 1;
    }
    
    return 0;
}
