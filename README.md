# CGTL (C Generic Template Library)

CGTL 是一个参考 C++ STL 设计的 C 语言容器与算法库。

当前实现包含：

- 序列式容器
- 有序关联式容器
- 无序关联式容器
- 容器适配器
- 字符串容器
- 统一迭代器与 range 抽象
- 基于宏模板实例化的通用算法

---

## 目录结构

```text
include/
├── cgtl.h                  # 公共宏和辅助定义
序列式容器（Sequence Containers）
├── array.h                 # 固定大小数组
├── vector.h                # 动态数组
├── forward_list.h          # 单向链表
├── list.h                  # 双向链表
├── deque.h                 # 双端队列
关联式容器（Associative Containers）
├── set.h                   # 集合
├── multiset.h              # 多重集合
├── map.h                   # 映射
├── multimap.h              # 多重映射
无序关联式容器（Unordered Associative Containers）
├── unordered_set.h         # 无序集合
├── unordered_multiset.h    # 无序多重集合
├── unordered_map.h         # 无序映射
├── unordered_multimap.h    # 无序多重映射
容器适配器（Container Adapters）
├── stack.h                 # 栈
├── queue.h                 # 队列
├── pri_queue.h             # 优先队列
算法与迭代器（Algorithms and Iterators）
├── iterator.h              # 统一 iterator / range / iterator category
├── algorithm.h             # 按 T 实例化的通用算法
辅助容器（Auxiliary Containers）
├── str.h                   # 基于 vector<char> 的字符串
├── cstring.h               # 独立动态字符串
└── README.md
```

---

## 快速开始

### 使用容器

```c
#define T int
#include "vector.h"

int main(void)
{
    vec_int v = vec_int_init();

    vec_int_push_back(&v, 3);
    vec_int_push_back(&v, 1);
    vec_int_push_back(&v, 2);

    printf("front = %d\n", *vec_int_front(&v));
    printf("size  = %zu\n", v.size);

    vec_int_free(&v);
    return 0;
}
```

### 使用不同容器

容器宏使用 `T` 作为元素类型。切换容器时重新定义 `T` 即可：

```c
#define T int
#include "vector.h"
#undef T

#define T int
#include "deque.h"
#undef T
```

同一个容器类型在同一个翻译单元中不要重复实例化。

---

## 统一迭代器

`iterator.h` 提供：

- `cgtl_iter_category`
- `cgtl_iter`
- `cgtl_iter_ops`
- `cgtl_range`
- `cgtl_out_iter`
- 迭代器前进、比较、距离和定位辅助函数

所有可遍历容器都提供统一形式的接口：

```c
cgtl_range vec_int_iter_range(vec_int *self);
cgtl_range lst_int_iter_range(lst_int *self);
cgtl_range set_int_iter_range(set_int *self);
cgtl_range map_int_int_iter_range(map_int_int *self);
cgtl_range ust_int_iter_range(ust_int *self);
```

基本用法：

```c
cgtl_range range = vec_int_iter_range(&v);

for(cgtl_iter it = range.first;
    !cgtl_iter_equal(&it, &range.last);
    cgtl_iter_step(&it))
{
    int *value = (int *)it.ref;
    printf("%d\n", *value);
}
```

### 迭代器能力

| 类别 | 容器 |
|---|---|
| 随机访问 | `array`、`vector`、`deque`、`str`、`cstring` |
| 双向 | `list`、`set`、`multiset`、`map`、`multimap` |
| 单向 | `forward_list`、`unordered_set`、`unordered_map`、`unordered_multiset`、`unordered_multimap` |
| 不提供通用遍历迭代器 | `stack`、`queue`、`pri_queue` |

`unordered_multimap` 目前的统一迭代器 `ref` 指向 key；如果需要遍历完整的 key/value 对，需要进一步调整其节点视图。

---

## 通用算法

`algorithm.h` 是按元素类型 `T` 实例化的模板头文件，不提供普通的单次 include guard。

### 同类型算法

```c
#define T int
#define HOLD
#include "vector.h"
#include "algorithm.h"

static int compare_int(int *a, int *b)
{
    return (*a > *b) - (*a < *b);
}

static void print_int(int *value)
{
    printf("%d\n", *value);
}

int main(void)
{
    vec_int v = vec_int_init();
    vec_int_push_back(&v, 3);
    vec_int_push_back(&v, 1);
    vec_int_push_back(&v, 2);

    cgtl_range range = vec_int_iter_range(&v);

    algo_int_sort(range, compare_int);
    algo_int_for_each(range, print_int);

    vec_int_free(&v);
    return 0;
}
```

当前提供的算法包括：

- 遍历与判定：`for_each`、`all_of`、`any_of`、`none_of`
- 查找与计数：`find`、`find_if`、`find_if_not`、`count`、`count_if`
- 比较：`equal`、`lexicographical_compare`
- 修改：`copy`、`copy_if`、`transform`、`fill`、`replace`、`replace_if`
- 删除与压缩：`remove`、`remove_if`、`unique`
- 最值：`min_element`、`max_element`
- 反转与排序：`reverse`、`sort`、`is_sorted`
- 有序查找：`lower_bound`、`upper_bound`、`binary_search`
- 集合运算：`set_union`、`set_intersection`、`set_difference`、`set_symmetric_difference`
- 输出迭代器版本：`copy_to`、`transform_to`

### 跨类型算法

跨类型算法使用可选的 `U`：

```c
#define T int
#define U double
#include "algorithm.h"
```

生成：

```text
algo_int_to_double_copy
algo_int_to_double_transform
```

### 算法前缀

公共使用生成 `algo_<T>_*`：

```text
algo_int_find
algo_int_sort
algo_int_binary_search
```

容器内部会生成独立前缀，避免多个容器使用同一个 `T` 时重复定义：

```text
vec_int_algo_find
lst_int_algo_find
set_int_algo_equal
map_int_int_algo_equal
```

---

## 兼容包装

容器原有的通用成员算法接口仍然保留，但实现已经改为委托给算法层。

| 容器 | 已改为兼容包装的旧接口 |
|---|---|
| `array` | `ARRAY_equal`、`ARRAY_find`、`ARRAY_count`、`ARRAY_remove_if` |
| `vector` | `VEC_equal`、`VEC_find`、`VEC_sort`、`VEC_ranged_sort`、`VEC_remove_if` |
| `deque` | `DEQ_equal`、`DEQ_find`、`DEQ_sort`、`DEQ_ranged_sort`、`DEQ_remove_if` |
| `list` | `LST_equal`、`LST_find` |
| `forward_list` | `FLST_find` |
| `set` / `multiset` | `SET_equal`、`SET_union`、`SET_intersection`、`SET_difference`、`SET_symmetric_difference` |

以下操作继续保留在容器内部，因为它们需要操作节点、桶或树结构：

- `list` / `forward_list` 的 `sort`、`merge`、`splice`、`reverse`、`unique`、`remove_if`
- `map` / `set` / `unordered_*` 的专用查找接口
- `unordered_*` 的桶、负载因子和 rehash 接口
- `stack`、`queue`、`pri_queue` 的适配器操作
- 字符串的 `find`、`rfind`、`substr`、`compare` 等专用接口

### 排序兼容说明

旧测试中存在返回 `0/1` 布尔值的比较器，而标准算法要求返回 `<0/0/>0` 三态值。

为了保持旧接口行为：

- `VEC_sort`、`VEC_ranged_sort`、`DEQ_sort`、`DEQ_ranged_sort` 使用 `legacy_sort` 兼容包装
- 新代码应使用 `algo_T_sort`
- 新代码中的比较器应返回负数、0 或正数

例如：

```c
static int compare_int(int *a, int *b)
{
    return (*a > *b) - (*a < *b);
}
```

---

## 构建与测试

编译测试：

```bash
make clean
make -j4 run_tests
```

运行单个测试：

```bash
make test_vector
make test_list
make test_deque
make test_set
make test_iterator_algorithm
```

`run_tests` 会在任一测试失败时返回非零状态。

当前测试覆盖：

- `vector`
- `list`
- `deque`
- `queue`
- `stack`
- `pri_queue`
- `set`
- `unordered_set`
- `map`
- `unordered_map`
- `multiset`
- `multimap`
- `unordered_multiset`
- `unordered_multimap`
- `array`
- `forward_list`
- `cstring`
- `iterator/algorithm`

---

## 示例

- `example/vector_autodiff.c`：vector 自动微分示例
- `example/algorithm_demo.c`：iterator 和 algorithm 新接口示例

`example/algorithm_demo.c` 会演示：

- 获取 vector/list 的统一 range
- 使用 `algo_int_for_each`
- 使用 `algo_int_sort`
- 使用 `algo_int_find`
- 使用 `algo_int_count`

---

## 注意事项

- `algorithm.h` 是 `T` 参数化头文件，应根据需要为每个元素类型单独 include。
- 公共算法使用 `algo_<T>_*`，容器内部算法使用 `algo_<container>_<T>_*`。
- `sort` 只支持随机访问 range；`list` 和 `forward_list` 使用自己的节点排序。
- `unordered` 容器不提供排序接口。
- `stack`、`queue`、`pri_queue` 按 STL 适配器设计，不暴露通用遍历迭代器。
